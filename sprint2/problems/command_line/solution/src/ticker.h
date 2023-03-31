#pragma once
//#include <iostream>
//#include <boost/beast/http/verb.hpp>
#include <string_view>
//#include <map>
//#include <string>
//#include <filesystem>
//#include "event_logger.h"
#include <functional>
#include <boost/asio/io_context.hpp>
#include <memory>
#include <chrono>

namespace net = boost::asio;
namespace sys = boost::system;

namespace http_handler {

class Ticker : public std::enable_shared_from_this<Ticker> {
public:
    using Strand = net::strand<net::io_context::executor_type>;
    using Handler = std::function<void(std::chrono::milliseconds delta)>;

    Ticker(Strand strand, std::chrono::milliseconds period, Handler handler)
    :strand_(strand), period_(period), handler_(handler)  {
//    	std::cout << "Ticker:" << period_.count() << std::endl;
    }

    void Start() {
  //  	std::cout << "Ticker: Start" << std::endl;
        last_tick_ = std::chrono::steady_clock::now();
        /* Выполнить SchedulTick внутри strand_ */
        net::dispatch(strand_, [self = shared_from_this()] {
             self->ScheduleTick();
         });
    }
private:
    void ScheduleTick() {
 //   	std::cout << "Ticker: Schedule tick" << std::endl;
        /* выполнить OnTick через промежуток времени period_ */
        timer_.expires_after(period_);
        timer_.async_wait([self = shared_from_this()](sys::error_code ec)
        		{
        			self->OnTick(ec);
        		});
    }

    void OnTick(sys::error_code ec) {
  //  		std::cout << "on tick" << std::endl;

            auto current_tick = std::chrono::steady_clock::now();
            auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(current_tick - last_tick_);
            handler_(delta);
            last_tick_ = current_tick;
            ScheduleTick();
        }


    Strand strand_;
    net::steady_timer timer_{strand_};
    std::chrono::milliseconds period_;
    Handler handler_;
    std::chrono::time_point<std::chrono::steady_clock> last_tick_;
};
}
