#include "sdk.h"
//
#include <boost/asio/signal_set.hpp>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "http_server.h"

#include <boost/beast/http/message.hpp>
#include <boost/beast/http/verb.hpp>
#include <map>
#include <functional>

namespace {
namespace net = boost::asio;
using namespace std::literals;
namespace sys = boost::system;
namespace http = boost::beast::http;

// Запрос, тело которого представлено в виде строки
using StringRequest = http::request<http::string_body>;
// Ответ, тело которого представлено в виде строки
using StringResponse = http::response<http::string_body>;

struct ContentType {
    ContentType() = delete;
    constexpr static std::string_view TEXT_HTML = "text/html"sv;
    // При необходимости внутрь ContentType можно добавить и другие типы контента
};

struct AllowType {
    AllowType() = delete;
    constexpr static std::string_view GET_HEAD = "GET, HEAD"sv;
    // При необходимости внутрь ContentType можно добавить и другие типы контента
};

// Создаёт StringResponse с заданными параметрами
StringResponse MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
                                  bool keep_alive, const std::map<http::field, std::string_view>& addition_headers = {},
                                  std::string_view content_type = ContentType::TEXT_HTML) {
    StringResponse response(status, http_version);
    response.set(http::field::content_type, content_type);
    
    for(auto it = addition_headers.begin(); it != addition_headers.end(); ++it)
	response.set(it->first, it->second);
    
    response.body() = body;
    response.content_length(body.size());
    response.keep_alive(keep_alive);
    return response;
}

std::string GetResponce(std::string_view trgt)
{
   trgt.remove_prefix(1);
  
   std::string str = {trgt.begin(), trgt.end()};
   return "Hello, " + str;
}

StringResponse HandleRequest(StringRequest&& req) {
   using namespace std::literals;
   const auto get_responce = [&req](){
	    return MakeStringResponse(http::status::ok, GetResponce(req.target()), req.version(), req.keep_alive());
   };

   const auto head_responce = [&req](){
	   return MakeStringResponse(http::status::ok, ""sv, req.version(), req.keep_alive());
   };
   
   const auto invalid_method = [&req](){
	   return MakeStringResponse(http::status::method_not_allowed, "Invalid method"sv, req.version(),
	   			      req.keep_alive(), {{http::field::allow, AllowType::GET_HEAD}});
   };

   std::map<boost::beast::http::verb, std::function<StringResponse()>>
   verbHandlers{{http::verb::get, get_responce}, {http::verb::head, head_responce}};

  auto itFound = verbHandlers.find(req.method());
  if(itFound != verbHandlers.end())
	return itFound->second();

   return invalid_method();
}

// Запускает функцию fn на n потоках, включая текущий
template <typename Fn>
void RunWorkers(unsigned n, const Fn& fn) {
    n = std::max(1u, n);
    std::vector<std::jthread> workers;
    workers.reserve(n - 1);
    // Запускаем n-1 рабочих потоков, выполняющих функцию fn
    while (--n) {
        workers.emplace_back(fn);
    }
    fn();
}

}  // namespace

int main() {
    const unsigned num_threads = std::thread::hardware_concurrency();

    net::io_context ioc(num_threads);

    // Подписываемся на сигналы и при их получении завершаем работу сервера
    net::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait([&ioc](const sys::error_code& ec, [[maybe_unused]] int signal_number) {
        if (!ec) {
            ioc.stop();
        }
    });

    const auto address = net::ip::make_address("0.0.0.0");
    constexpr net::ip::port_type port = 8080;
    http_server::ServeHttp(ioc, {address, port}, [](auto&& req, auto&& sender) {
         sender(HandleRequest(std::forward<decltype(req)>(req)));
    });

    // Эта надпись сообщает тестам о том, что сервер запущен и готов обрабатывать запросы
    std::cout << "Server has started..."sv << std::endl;

    RunWorkers(num_threads, [&ioc] {
        ioc.run();
    });

}
