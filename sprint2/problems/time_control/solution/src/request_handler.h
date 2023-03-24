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

namespace net = boost::asio;

namespace http_handler {

namespace beast = boost::beast;
namespace http = beast::http;
namespace sys = boost::system;

 using namespace std::literals;

 using StringResponse = http::response<http::string_body>;
 using StaticFileResponce = http::response<http::file_body>;
 
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


http::response<http::file_body> MakeFileResponce(const std::filesystem::path& json_path, const std::string& mime_type);
std::string_view GetFileExtension(std::string_view path);
std::string GetMimeType(std::string_view extension);
std::string GetAuthToken(std::string_view auth);

class RequestHandler {
public:
    explicit RequestHandler(model::Game& game)
        : game_{game} {
    }

    RequestHandler(const RequestHandler&) = delete;
    RequestHandler& operator=(const RequestHandler&) = delete;

    StringResponse MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
                                      bool keep_alive, std::string_view content_type = ContentType::APPLICATION_JSON,
                                      const std::initializer_list< std::pair<http::field, std::string_view> >& addition_headers = {});

    StringResponse HandleAuthRequest(const std::string& body, unsigned http_version, bool keep_alive);
    StringResponse HandleGetPlayersRequest(http::verb method, std::string_view auth_type, const std::string& body, unsigned http_version, bool keep_alive);
    StringResponse HandleGetGameState(http::verb method, std::string_view auth_type, const std::string& body, unsigned http_version, bool keep_alive);
    StringResponse HandlePlayerAction(http::verb method, std::string_view auth_type, const std::string& body, unsigned http_version, bool keep_alive);
    StringResponse HandleTickAction(http::verb method, std::string_view auth_type, const std::string& body, unsigned http_version, bool keep_alive);

    template <typename Body, typename Allocator, typename Send>
    void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {
    	if(!req.target().compare("/api/v1/game/join"sv))
    	{
//    		std::cout << "Request to join game:" <<std::endl;
    		if(req.method() == http::verb::post)
    		{
    			auto resp = HandleAuthRequest(req.body(), req.version(),req.keep_alive());
    			send(std::move(resp));
    		}
    		else
    	    	if(req.method() == http::verb::head)
    	    	 {
    	    		auto resp = MakeStringResponse(http::status::method_not_allowed,""sv,
    	    									   req.version(), req.keep_alive(), ContentType::APPLICATION_JSON, {{http::field::cache_control, "no-cache"sv}, {http::field::allow, HeaderType::ALLOW_POST}});
    	    	  	send(std::move(resp));
    	    	 }
    		else
    		{
    			auto resp = MakeStringResponse(http::status::method_not_allowed,
    			    					       json_serializer::MakeMappedResponce({ {"code", "invalidMethod"},
    																		  {"message", "Only POST method is expected"}}),
    										   req.version(), req.keep_alive(), ContentType::APPLICATION_JSON, {{http::field::cache_control, "no-cache"sv}, {http::field::allow, HeaderType::ALLOW_POST}});
    			send(std::move(resp));
    		}
    	}
    	else
    		if(!req.target().compare("/api/v1/game/players"sv))
    		{
    			auto resp = HandleGetPlayersRequest(req.method(), req[http::field::authorization], req.body(), req.version(),req.keep_alive());
    			send(std::move(resp));
    		}
    		else
    			if(!req.target().compare("/api/v1/game/state"sv))
    			   {
    				auto resp = HandleGetGameState(req.method(), req[http::field::authorization], req.body(), req.version(),req.keep_alive());
    			    send(std::move(resp));
    			   }
    				else
    			    if(!req.target().compare("/api/v1/game/player/action"sv))
    			    {
    			    	auto resp = HandlePlayerAction(req.method(), req[http::field::authorization], req.body(), req.version(),req.keep_alive());
    			        send(std::move(resp));
    			    }
    			    else
    			    	if(!req.target().compare("/api/v1/game/tick"sv))
    			    	{
    			    		auto resp = HandleTickAction(req.method(), req[http::field::authorization], req.body(), req.version(),req.keep_alive());
    			    		send(std::move(resp));
    			    	}
    		else
    		{
    			if( req.method() != http::verb::get)
    				return;
    			std::string_view parameters = req.target();
    		 	std::string_view apiPrefix = "/api/";
    			std::string_view fullPrefix = "/api/v1/maps";

    			const std::string uri = {parameters.begin(), parameters.end()};
    		        event_logger::LogServerRequestReceived("127.0.0.1", uri, "GET");

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
    		            else
    		            {
    		            	if(!parameters.starts_with(apiPrefix))
    		            	{
    			            	if((parameters.size() == 1) && parameters.starts_with('/'))
    			            		parameters = "/index.html";

    		            		auto extension = GetFileExtension(parameters);
    		            		std::string mimeType = GetMimeType(extension);

    		            		std::filesystem::path pathTrail = parameters;
    					        std::filesystem::path basePath = game_.GetBasePath();
    					        basePath += pathTrail;

    					        try{
    					        	auto fileResp = MakeFileResponce(basePath, mimeType);
    					        	send(std::move(fileResp));
    					        	event_logger::LogServerRespondSend(1000, static_cast<unsigned>(http::status::ok), mimeType);
    					        }
    					        catch(const std::filesystem::filesystem_error& ex)
    					        {
    					        	auto notFoundResp = MakeStringResponse(http::status::not_found,  json_serializer::MakeMapNotFoundResponce(), req.version(), req.keep_alive(), ContentType::TEXT_PLAIN);
    					        	send(std::move(notFoundResp));
    					        }
    		            	}
    		           }
    		        }

    		}
    }
private:
    model::Game& game_;
    std::map<http::verb, std::function<void()>> handlers_;
};

class SyncWriteOStreamAdapter {
public:
    explicit SyncWriteOStreamAdapter(std::ostream& os)
        : os_{os} {
    }

    template <typename ConstBufferSequence>
    size_t write_some(const ConstBufferSequence& cbs, sys::error_code& ec) {
        const size_t total_size = net::buffer_size(cbs);
        if (total_size == 0) {
            ec = {};
            return 0;
        }
        size_t bytes_written = 0;
        for (const auto& cb : cbs) {
            const size_t size = cb.size();
            const char* const data = reinterpret_cast<const char*>(cb.data());
            if (size > 0) {
                if (!os_.write(reinterpret_cast<const char*>(data), size)) {
                    ec = make_error_code(boost::system::errc::io_error);
                    return bytes_written;
                }
                bytes_written += size;
            }
        }
        ec = {};
        return bytes_written;
    }

    template <typename ConstBufferSequence>
    size_t write_some(const ConstBufferSequence& cbs) {
        sys::error_code ec;
        const size_t bytes_written = write_some(cbs, ec);
        if (ec) {
            throw std::runtime_error("Failed to write");
        }
        return bytes_written;
    }

private:
    std::ostream& os_;
}; 

}  // namespace http_handler
