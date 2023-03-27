#include "sdk.h"
//
#include <boost/asio/io_context.hpp>
#include <iostream>
#include <thread>
#include <boost/asio/signal_set.hpp>
#include "json_loader.h"
#include "json_serializer.h"
#include "request_handler.h"
#include <filesystem>
#include "event_logger.h"

using namespace std::literals;
namespace net = boost::asio;
namespace sys = boost::system;
namespace {

// Запускает функцию fn на num_threads потоках, включая текущий
template <typename Fn>
void RunWorkers(unsigned num_threads, const Fn& fn) {
	num_threads = std::max(1u, num_threads);
    std::vector<std::jthread> workers;
    workers.reserve(num_threads - 1);
    // Запускаем num_threads-1 рабочих потоков, выполняющих функцию fn
    while (--num_threads) {
        workers.emplace_back(fn);
    }
    fn();
}

}  // namespace

int main(int argc, const char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: game_server <game-config-json> <content-base-dir-json>"sv << std::endl;
        return EXIT_FAILURE;
    }

    try {
        // 1. Загружаем карту из файла и построить модель игры
        model::Game game = json_loader::LoadGame(argv[1], argv[2]);//"../../data/config.json", "../../static");//argv[1], argv[2]);

        // 2. Инициализируем io_context
        const unsigned num_threads = std::thread::hardware_concurrency();
        net::io_context ioc(num_threads);

        // 3. Добавляем асинхронный обработчик сигналов SIGINT и SIGTERM
	// Подписываемся на сигналы и при их получении завершаем работу сервера
	net::signal_set signals(ioc, SIGINT, SIGTERM);
	signals.async_wait([&ioc](const sys::error_code& ec, [[maybe_unused]] int signal_number) {
        	if (!ec) {
	            ioc.stop();
	            event_logger::LogServerEnd("server exited", EXIT_SUCCESS);
	        }
	});

        // 4. Создаём обработчик HTTP-запросов и связываем его с моделью игры
       // http_handler::RequestHandler handler{game, ioc};

        auto handler = std::make_shared<http_handler::RequestHandler>(game, ioc);
  //      http_server::ServeHttp(ioc, {address, port}, [&handler](auto&& req, auto&& send)
  //      { handler->operator()(std::forward<decltype(req)>(req), std::forward<decltype(send)>(send)); });

        // 5. Запустить обработчик HTTP-запросов, делегируя их обработчику запросов
        const auto address = net::ip::make_address("0.0.0.0");
        constexpr net::ip::port_type port = 8080;
        
        http_server::ServeHttp(ioc, {address, port}, [&handler](auto&& req, auto&& send) {
            (*handler)(std::forward<decltype(req)>(req), std::forward<decltype(send)>(send));
            //handler->operator()(std::forward<decltype(req)>(req), std::forward<decltype(send)>(send));
        });
        
	event_logger::InitLogger();
        // Эта надпись сообщает тестам о том, что сервер запущен и готов обрабатывать запросы
        //std::cout << "Server has started..."sv << std::endl;
        event_logger::LogStartServer(address.to_string(), port, "server started");

        // 6. Запускаем обработку асинхронных операций
        RunWorkers(std::max(1u, num_threads), [&ioc] {
            ioc.run();
        });
    } catch (const std::exception& ex) {
        event_logger::LogServerEnd("server exited", EXIT_FAILURE, ex.what());
        return EXIT_FAILURE;
    }
}
