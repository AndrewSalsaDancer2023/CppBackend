#ifdef WIN32
#include <sdkddkver.h>
#endif
// boost.beast будет использовать std::string_view вместо boost::string_view
#define BOOST_BEAST_USE_STD_STRING_VIEW

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <iostream>
#include <thread>
#include <optional>

#include <boost/asio/write.hpp>

#include <boost/beast/http/message.hpp>
#include <boost/beast/http/verb.hpp>
#include <map>
#include <functional>


namespace net = boost::asio;
using tcp = net::ip::tcp;
using namespace std::literals;
namespace beast = boost::beast;
namespace http = beast::http;

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

std::optional<StringRequest> ReadRequest(tcp::socket& socket, beast::flat_buffer& buffer) {
    beast::error_code ec;
    StringRequest req;
    // Считываем из socket запрос req, используя buffer для хранения данных.
    // В ec функция запишет код ошибки.
    http::read(socket, buffer, req, ec);

    if (ec == http::error::end_of_stream) {
        return std::nullopt;
    }
    if (ec) {
        throw std::runtime_error("Failed to read request: "s.append(ec.message()));
    }
    return req;
} 

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

template <typename RequestHandler>
void HandleConnection(tcp::socket& socket, RequestHandler&& handle_request) {
    try {
        // Буфер для чтения данных в рамках текущей сессии.
        beast::flat_buffer buffer;

        while (auto request = ReadRequest(socket, buffer)) {

	    StringResponse response = handle_request(*std::move(request));
            // Отправляем ответ сервера клиенту
            http::write(socket, response);

            // Прекращаем обработку запросов, если семантика ответа требует это
            if (response.need_eof()) {
                break;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    beast::error_code ec;
    // Запрещаем дальнейшую отправку данных через сокет
    socket.shutdown(tcp::socket::shutdown_send, ec);
}


int main() {  
    net::io_context ioc;

    const auto address = net::ip::make_address("0.0.0.0");
    constexpr unsigned short port = 8080;

    tcp::acceptor acceptor(ioc, {address, port});
    std::cout << "Server has started..." << std::endl;
    while (true) {
        tcp::socket socket(ioc);
        acceptor.accept(socket);
        HandleConnection(socket, HandleRequest);
    }
    
    return 0;
}
