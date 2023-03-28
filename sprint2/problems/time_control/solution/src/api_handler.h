#pragma once
#include "http_server.h"
#include "model.h"
#include <iostream>
#include <boost/beast/http/verb.hpp>
#include <string_view>
#include "json_serializer.h"
#include "json_loader.h"
#include <map>
#include <string>
#include <filesystem>
#include "event_logger.h"
#include <functional>
#include "server_exceptions.h"
#include <boost/asio/io_context.hpp>
#include <cassert>
namespace net = boost::asio;

namespace http_handler {

namespace beast = boost::beast;
namespace http = beast::http;
namespace sys = boost::system;

using namespace std::literals;

using StringResponse = http::response<http::string_body>;

struct ContentType {
    ContentType() = delete;
    constexpr static std::string_view APPLICATION_JSON = "application/json"sv;
    constexpr static std::string_view TEXT_PLAIN = "text/plain"sv;
};

struct HeaderType {
	HeaderType() = delete;
    constexpr static std::string_view ALLOW_HEADERS = "GET, HEAD"sv;
    constexpr static std::string_view ALLOW_POST = "POST"sv;
};

StringResponse MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
                                  bool keep_alive, std::string_view content_type = ContentType::APPLICATION_JSON,
                                  const std::initializer_list< std::pair<http::field, std::string_view> >& addition_headers = {});



class ApiHandler{
public:
     explicit ApiHandler(model::Game& game):game_{game}{
        InitApiRequestHandlers();
    }

    ApiHandler(const ApiHandler&) = delete;
    ApiHandler& operator=(const ApiHandler&) = delete;
    
    bool IsApiRequest(const std::string& request);
    StringResponse HandleApiRequest(const std::string& request, http::verb method, std::string_view auth_type, const std::string& body, unsigned http_version, bool keep_alive);
/*    template <typename Body, typename Allocator, typename Send>
        void HandleRequest(const std::string& request, http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send)
    {
    	auto resp = HandleApiRequest(request, req.method(), req[http::field::authorization], req.body(), req.version(),req.keep_alive());
    	send(std::move(resp));
    }
  */  
private:
    void InitApiRequestHandlers();
    StringResponse HandleJoinGameRequest(http::verb method, std::string_view auth_type, const std::string& body, unsigned http_version, bool keep_alive);
/*    StringResponse MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
                                      bool keep_alive, std::string_view content_type = ContentType::APPLICATION_JSON,
                                      const std::initializer_list< std::pair<http::field, std::string_view> >& addition_headers = {});
*/
    StringResponse HandleAuthRequest(const std::string& body, unsigned http_version, bool keep_alive);
    StringResponse HandleGetPlayersRequest(http::verb method, std::string_view auth_type, const std::string& body, unsigned http_version, bool keep_alive);
    StringResponse HandleGetGameState(http::verb method, std::string_view auth_type, const std::string& body, unsigned http_version, bool keep_alive);
    StringResponse HandlePlayerAction(http::verb method, std::string_view auth_type, const std::string& body, unsigned http_version, bool keep_alive);
    StringResponse HandleTickAction(http::verb method, std::string_view auth_type, const std::string& body, unsigned http_version, bool keep_alive);
private:
    model::Game& game_;
    std::map<std::string, std::function<StringResponse(http::verb, std::string_view, const std::string&, unsigned, bool)>> resp_map_;
};    
}  // namespace http_handler
