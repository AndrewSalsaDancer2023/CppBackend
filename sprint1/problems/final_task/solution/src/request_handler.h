#pragma once
#include "http_server.h"
#include "model.h"
#include <iostream>
#include <boost/beast/http/verb.hpp>
#include <string_view>
#include "json_serializer.h"

namespace http_handler {

namespace beast = boost::beast;
namespace http = beast::http;

 using namespace std::literals;

 using StringResponse = http::response<http::string_body>;

struct ContentType {
    ContentType() = delete;
    constexpr static std::string_view APPLICATION_JSON = "application/json"sv;
};

class RequestHandler {
public:
    explicit RequestHandler(model::Game& game)
        : game_{game} {
    }

    RequestHandler(const RequestHandler&) = delete;
    RequestHandler& operator=(const RequestHandler&) = delete;

    StringResponse MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
                                  bool keep_alive, std::string_view content_type = ContentType::APPLICATION_JSON);

    template <typename Body, typename Allocator, typename Send>
    void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {
        // Обработать запрос request и отправить ответ, используя send
	if( req.method() != http::verb::get)
            return;
	
	std::string_view parameters = req.target();
	std::string_view apiPrefix = "/api/";
	std::string_view fullPrefix = "/api/v1/maps";

	StringResponse resp;
	if(parameters.starts_with(apiPrefix) && !parameters.starts_with(fullPrefix))
	{
            resp = MakeStringResponse(http::status::bad_request, json_serializer::MakeBadRequestResponce(),	 			      		        req.version(), req.keep_alive());
            send(std::move(resp));
	}
	else
	{
            if(parameters.starts_with(fullPrefix))
            {
                parameters.remove_prefix(fullPrefix.size());
                if(parameters.empty())
                {
                    resp = MakeStringResponse(http::status::ok, json_serializer::GetMapListResponce(game_), 						req.version(), req.keep_alive());
                }
                else
                {
                    parameters.remove_prefix(1);
                    const auto& responce = json_serializer::GetMapContentResponce(game_, {parameters.begin(), parameters.end()});
                    if(!responce.empty())
                    {
                        resp = MakeStringResponse(http::status::ok, responce, 						           req.version(), req.keep_alive());
                    }
                    else
                    {
                        resp = MakeStringResponse(http::status::not_found, json_serializer::MakeMapNotFoundResponce(), 							    req.version(), req.keep_alive());
                    }
                }
                send(std::move(resp));
            }
        }
		
    }
    
private:
    model::Game& game_;
};

}  // namespace http_handler
