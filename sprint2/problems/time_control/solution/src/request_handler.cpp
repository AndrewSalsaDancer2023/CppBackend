#include "request_handler.h"
//#include <boost/algorithm/string.hpp>
#include <boost/beast.hpp>
#include <iostream>
#include <string_view>
#include <set>

using namespace std::literals;
namespace fs = std::filesystem;

namespace beast = boost::beast;
namespace http = beast::http;
namespace sys = boost::system;

namespace http_handler {

std::map <std::string_view, std::string> extensionToMime {
{".htm", "text/html"}, 
{".html", "text/html"},
{".css", "text/css"},
{".txt", "text/plain"},
{".js", "text/javascript"},
{".json", "application/json"},
{".xml", "application/xml"},
{".png", "image/png"},
{".jpg", "image/jpeg"},
{".jpe", "image/jpeg"},
{".jpeg", "image/jpeg"},
{".gif", "image/gif"},
{".bmp", "image/bmp"},
{".ico", "image/vnd.microsoft.icon"},
{".tiff", "image/tiff"},
{".tif", "image/tiff"},
{".svg", "image/svg+xml"},
{".svgz", "image/svg+xml"},
{".mp3", "audio/mpeg"}
};
/*
const std::string game_endpoint = "/api/v1/game/join";
const std::string players_endpoint = "/api/v1/game/players";
const std::string state_endpoint = "/api/v1/game/state";
const std::string action_endpoint = "/api/v1/game/player/action";
const std::string tick_endpoint = "/api/v1/game/tick";
*/
std::string GetMimeType(std::string_view extension)
{
  auto it = extensionToMime.find(extension);
  if(it != extensionToMime.end())	
	return it->second;
   return "";
}

std::string_view GetFileExtension(std::string_view path)
{
  auto dotPosition = path.find_last_of('.');
  if(dotPosition != std::string_view::npos)
  {
	auto extension = path.substr(dotPosition);
	if(!extension.empty())
	  return extension;
  }
  return "";
}

/*
std::string GetAuthToken(std::string_view auth)
{
	std::string_view prefix = "Bearer"sv;

	if(auth.find_first_of(prefix) == std::string_view::npos)
	{
		return "";
	}
	auth.remove_prefix(prefix.size());

	size_t pos = 0;
	do{
		pos = auth.find_first_of(' ');
		if(pos == 0)
			auth.remove_prefix(1);
	}while(pos != std::string_view::npos);

	return std::string(auth.begin(), auth.end());
}

bool RequestHandler::IsApiRequest(const std::string& request)
{
	std::set<std::string> api_endpoints {game_endpoint, players_endpoint,  state_endpoint, action_endpoint, tick_endpoint};

	return api_endpoints.find(request) != api_endpoints.end();
}

StringResponse RequestHandler::HandleApiRequest(const std::string& request, http::verb method, std::string_view auth_type, const std::string& body, unsigned http_version, bool keep_alive)
{
	StringResponse resp;
	auto it_handler = resp_map_.find(request);
	if(it_handler != resp_map_.end())
		return it_handler->second(method, auth_type, body, http_version, keep_alive);

	return resp;
}

void RequestHandler::InitApiRequestHandlers()
{
	auto join_game_handler = [this](http::verb method, std::string_view auth_type, const std::string& body, unsigned http_version, bool keep_alive) -> StringResponse
				 {
			 	 	 return HandleJoinGameRequest(method, auth_type, body, http_version, keep_alive);
				 };

	auto get_players_handler = [this](http::verb method, std::string_view auth_type, const std::string& body, unsigned http_version, bool keep_alive) -> StringResponse
				 {
			 	 	 return HandleGetPlayersRequest(method, auth_type, body, http_version, keep_alive);
				 };

	auto get_state_handler = [this](http::verb method, std::string_view auth_type, const std::string& body, unsigned http_version, bool keep_alive) -> StringResponse
				 {
			 	 	 return HandleGetGameState(method, auth_type, body, http_version, keep_alive);
				 };

	auto action_handler = [this](http::verb method, std::string_view auth_type, const std::string& body, unsigned http_version, bool keep_alive) -> StringResponse
				 {
			 	 	  return HandlePlayerAction(method, auth_type, body, http_version, keep_alive);
				 };

	auto tick_handler = [this](http::verb method, std::string_view auth_type, const std::string& body, unsigned http_version, bool keep_alive) -> StringResponse
				{
			 	  	return  HandleTickAction(method, auth_type, body, http_version, keep_alive);
				};

	resp_map_[game_endpoint] = join_game_handler;
	resp_map_[players_endpoint] = get_players_handler;
	resp_map_[state_endpoint] = get_state_handler;
	resp_map_[action_endpoint] = action_handler;
	resp_map_[tick_endpoint] = tick_handler;
}
*/
/*
void RequestHandler::HandleFileRequest(const std::string_view target, http::verb method, std::string_view auth_type, const std::string& body, unsigned http_version, bool keep_alive)
{
	if(method != http::verb::get)
	  {
	  	auto resp = MakeStringResponse(http::status::method_not_allowed,
	  		  	    				   json_serializer::MakeMappedResponce({ {"code", "invalidMethod"}, {"message", "Invalid method"}}),
									   http_version, keep_alive, ContentType::APPLICATION_JSON);
	  	send(std::move(resp));
		return;
	  }

	std::string_view parameters = target;
	std::string_view apiPrefix = "/api/";
	std::string_view fullPrefix = "/api/v1/maps";

	const std::string uri = {parameters.begin(), parameters.end()};
	event_logger::LogServerRequestReceived(uri, "GET");

	StringResponse resp;
	if(parameters.starts_with(apiPrefix) && !parameters.starts_with(fullPrefix))
	  {
	        resp = MakeStringResponse(http::status::bad_request, json_serializer::MakeBadRequestResponce(),	http_version, keep_alive);
	        send(std::move(resp));
	        return;
	  }
      else
      {
    	  if(parameters.starts_with(fullPrefix))
	      {
    		  parameters.remove_prefix(fullPrefix.size());
    		  if(parameters.empty())
	    	  {
	    		  resp = MakeStringResponse(http::status::ok, json_serializer::GetMapListResponce(game_), http_version, keep_alive);
	    	  }
    		  else
	    	  {
	    		  parameters.remove_prefix(1);
	    		  const auto& responce = json_serializer::GetMapContentResponce(game_, {parameters.begin(), parameters.end()});
	    		  if(!responce.empty())
	    		  	{
	    			  resp = MakeStringResponse(http::status::ok, responce, http_version, keep_alive);
	    		    }
	    		    else
	    		   {
	    		      resp = MakeStringResponse(http::status::not_found, json_serializer::MakeMapNotFoundResponce(), http_version, keep_alive);
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
    			  		  auto notFoundResp = MakeStringResponse(http::status::not_found,  json_serializer::MakeMapNotFoundResponce(), http_version, keep_alive, ContentType::TEXT_PLAIN);
    			  		  send(std::move(notFoundResp));
    			  	  }
	    		}
	      }
      }

}
*/
/*
StringResponse RequestHandler::HandleJoinGameRequest(http::verb method, std::string_view auth_type, const std::string& body, unsigned http_version, bool keep_alive)
{
	StringResponse resp;
	if(method == http::verb::post)
	{
		resp = HandleAuthRequest(body, http_version, keep_alive);
	}
	else
    	if(method == http::verb::head)
    	{
    		resp = MakeStringResponse(http::status::method_not_allowed,""sv,
    								  http_version, keep_alive,
									  ContentType::APPLICATION_JSON,
									  {{http::field::cache_control, "no-cache"sv}, {http::field::allow, HeaderType::ALLOW_POST}});
    	}
    	else
    	{
    		resp = MakeStringResponse(http::status::method_not_allowed,
		    					       json_serializer::MakeMappedResponce({ {"code", "invalidMethod"},{"message", "Only POST method is expected"}}),
									   http_version, keep_alive, ContentType::APPLICATION_JSON,
								   {{http::field::cache_control, "no-cache"sv}, {http::field::allow, HeaderType::ALLOW_POST}});
    	}

	return resp;
}

StringResponse RequestHandler::MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
                                                   bool keep_alive, std::string_view content_type,
												   const std::initializer_list< std::pair<http::field, std::string_view> > & addition_headers)
 {
    StringResponse response(status, http_version);
    response.set(http::field::content_type, content_type);

    for(auto it = addition_headers.begin(); it != addition_headers.end(); ++it)
    	response.set(it->first, it->second);

    response.body() = body;
    response.content_length(body.size());
    response.keep_alive(keep_alive);
    return response;
 }
*/

StaticFileResponce MakeFileResponce(const std::filesystem::path& json_path, const std::string& mime_type)
 {
    http::response<http::file_body> res;
    res.version(11);  // HTTP/1.1
    res.result(http::status::ok);

    res.insert(http::field::content_type, mime_type);

    http::file_body::value_type file;

    if (sys::error_code ec; file.open(json_path.c_str(), beast::file_mode::read, ec), ec) {
       std::string reason = "Failed to open file: " + std::string(json_path.c_str());
       throw std::filesystem::filesystem_error(reason, ec);
    }

    res.body() = std::move(file);
    // Метод prepare_payload заполняет заголовки Content-Length и Transfer-Encoding
    // в зависимости от свойств тела сообщения
    res.prepare_payload();
    return res;
 }
/*
 StringResponse RequestHandler::HandleAuthRequest(const std::string& body, unsigned http_version, bool keep_alive)
     {
     	std::map<std::string, std::string> respMap;
     	try
     	{
     		respMap = json_loader::ParseJoinGameRequest(body);
     	}
     	catch(std::exception& e)
     	{
     		auto resp = MakeStringResponse(http::status::bad_request,
     		    									  json_serializer::MakeMappedResponce({ {"code", "invalidArgument"},
     		    																			{"message", "Join game request parse error"}}),
     																						http_version, keep_alive, ContentType::APPLICATION_JSON, {{http::field::cache_control, "no-cache"sv}});
     		return resp;
     	}
     	try
     	{
 //        	std::cout << "AddPlayer:" << std::endl;
     		auto [token, playerId] = game_.AddPlayer(respMap["mapId"], respMap["userName"]);
     		auto resp = MakeStringResponse(http::status::ok, json_serializer::MakeAuthResponce(token, playerId), http_version,
     									    keep_alive, ContentType::APPLICATION_JSON, {{http::field::cache_control, "no-cache"sv}});

     		return resp;
     	}
     	catch(MapNotFoundException& e)
     	{
     	     cout << e.what() << std::endl;
     	     auto resp = MakeStringResponse(http::status::not_found, json_serializer::MakeMapNotFoundResponce(), http_version, keep_alive,
     	    		 	 	 	 	 	 	ContentType::APPLICATION_JSON, {{http::field::cache_control, "no-cache"sv}});
     	     return resp;
     	}
     	catch(EmptyNameException& e)
     	{
 //   	        std::cout << e.what() << std::endl;
             auto resp = MakeStringResponse(http::status::bad_request, json_serializer::MakeMappedResponce({ {"code", "invalidArgument"},
 				  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  {"message", "Invalid name"}}),
             								http_version, keep_alive, ContentType::APPLICATION_JSON, {{http::field::cache_control, "no-cache"sv}});
    	        return resp;
     	}
     }

 StringResponse RequestHandler::HandleGetPlayersRequest(http::verb method, std::string_view auth_type, const std::string& body, unsigned http_version, bool keep_alive)
 {
 	//std::cout << "HandleGetPlayersRequest:" <<std::endl;

    if((method != http::verb::get) && (method != http::verb::head))
     {
 	    	auto resp = MakeStringResponse(http::status::method_not_allowed,
     		    	    					json_serializer::MakeMappedResponce({ {"code", "invalidMethod"},
     		    																  {"message", "Invalid method"}}),
 											http_version, keep_alive, ContentType::APPLICATION_JSON, {{http::field::cache_control, "no-cache"sv}, {http::field::allow, HeaderType::ALLOW_HEADERS}});
 	    	return resp;
     }


	std::string auth_token = GetAuthToken(auth_type);
 	//    				std::cout << "authToken:" << auth_token << std::endl;
	if(auth_token.empty())
	{
 	//    					std::cout << "authToken is empty" << std::endl;
		auto resp = MakeStringResponse(http::status::unauthorized,
 	  		    					   json_serializer::MakeMappedResponce({ {"code", "invalidToken"},
         																	  {"message", "Authorization header is missing"}}),
																			  http_version, keep_alive, ContentType::APPLICATION_JSON, {{http::field::cache_control, "no-cache"sv}});

		return resp;
	}
	if(!game_.HasSessionWithAuthInfo(auth_token))
	{
 	//    					std::cout << "No player win auth!" << std::endl;
		auto resp = MakeStringResponse(http::status::unauthorized,
									   json_serializer::MakeMappedResponce({ {"code", "unknownToken"},
 										        						     {"message", "Player token has not been found"}}),
										http_version, keep_alive, ContentType::APPLICATION_JSON, {{http::field::cache_control, "no-cache"sv}});
		return resp;
	}

 	//std::cout << "FindAllPlayersForAuthInfo:" << auth_token << std::endl;
	auto players =  game_.FindAllPlayersForAuthInfo(auth_token);
	StringResponse resp;
	if(method == http::verb::get)
		resp = MakeStringResponse(http::status::ok, json_serializer::GetPlayerInfoResponce(players), http_version, keep_alive, ContentType::APPLICATION_JSON, {{http::field::cache_control, "no-cache"sv}});
	else
		resp = MakeStringResponse(http::status::ok, "", http_version, keep_alive, ContentType::APPLICATION_JSON, {{http::field::cache_control, "no-cache"sv}});
	return resp;
 }

 bool IsValidAuthToken(const std::string& token, size_t valid_size)
 {
	 if(token.size() != valid_size)
		 return false;

	 for(auto i = 0; i < token.size(); ++i)
		 if((token[i] < '0') || (token[i] > '9') || (token[i] < 'a') || (token[i] > 'f'))
			 return false;

	 return true;
 }

 StringResponse RequestHandler::HandleGetGameState(http::verb method, std::string_view auth_type, const std::string& body, unsigned http_version, bool keep_alive)
 {
 	if((method != http::verb::get) && (method != http::verb::head))
 	{
 		auto resp = MakeStringResponse(http::status::method_not_allowed,
 	    					json_serializer::MakeMappedResponce({ {"code", "invalidMethod"},
 																  {"message", "Invalid method"}}),
																	  http_version, keep_alive, ContentType::APPLICATION_JSON, {{http::field::cache_control, "no-cache"sv}, {http::field::allow, HeaderType::ALLOW_HEADERS}});

 		return resp;
 	}
 	std::string auth_token = GetAuthToken(auth_type);
 	if(auth_token.empty() || !game_.HasSessionWithAuthInfo(auth_token))
		{
 		StringResponse resp;
 		if(auth_token.empty() || !IsValidAuthToken(auth_token, 32))
 			resp = MakeStringResponse(http::status::unauthorized,
 				    					json_serializer::MakeMappedResponce({ {"code", "invalidToken"},
        																		  {"message", "Authorization header is missing"}}),
   									    http_version, keep_alive, ContentType::APPLICATION_JSON, {{http::field::cache_control, "no-cache"sv}});
 		else
 			resp = MakeStringResponse(http::status::unauthorized,
 			    				    					json_serializer::MakeMappedResponce({ {"code", "unknownToken"},
 			           																		  {"message", "Player token has not been found"}}),
 			      									    http_version, keep_alive, ContentType::APPLICATION_JSON, {{http::field::cache_control, "no-cache"sv}});

 		return resp;
    }

		auto players =  game_.FindAllPlayersForAuthInfo(auth_token);
		if(method == http::verb::get)
		{
			auto resp = MakeStringResponse(http::status::ok, json_serializer::GetPlayersDogInfoResponce(players), http_version, keep_alive, ContentType::APPLICATION_JSON, {{http::field::cache_control, "no-cache"sv}});
			return resp;
		}
		else
		{
			auto resp = MakeStringResponse(http::status::ok, "", http_version, keep_alive, ContentType::APPLICATION_JSON, {{http::field::cache_control, "no-cache"sv}});
			return resp;
		}
 }

 StringResponse RequestHandler::HandlePlayerAction(http::verb method, std::string_view auth_type, const std::string& body, unsigned http_version, bool keep_alive)
 {
 	//std::cout << "Handle action" << std::endl;
 	if(method != http::verb::post)
 	{
 		auto resp = MakeStringResponse(http::status::method_not_allowed,
 	    					json_serializer::MakeMappedResponce({ {"code", "invalidMethod"},
 																  {"message", "Invalid method"}}),
																	  http_version, keep_alive, ContentType::APPLICATION_JSON, {{http::field::cache_control, "no-cache"sv}});

 		return resp;
 	}
 	std::string auth_token = GetAuthToken(auth_type);
 	if(auth_token.empty())
		{
	    	//    					std::cout << "authToken is empty" << std::endl;
			auto resp = MakeStringResponse(http::status::unauthorized,
 				    					json_serializer::MakeMappedResponce({ {"code", "invalidToken"},
        																		  {"message", "Authorization header is required"}}),
   									    http_version, keep_alive, ContentType::APPLICATION_JSON, {{http::field::cache_control, "no-cache"sv}});

 		return resp;
    }
 	else
 		if(!game_.HasSessionWithAuthInfo(auth_token))
 		{
 			auto resp = MakeStringResponse(http::status::unauthorized,
     				    					json_serializer::MakeMappedResponce({ {"code", "unknownToken"},
            																		  {"message", "Player token has not been found"}}),
       									    http_version, keep_alive, ContentType::APPLICATION_JSON, {{http::field::cache_control, "no-cache"sv}});

     		return resp;
 		}
 	//std::cout << "Set dog speed and direction" << std::endl;
 	auto player =  game_.GetPlayerWithAuthToken(auth_token);
 	auto session =  game_.GetSessionWithAuthInfo(auth_token);
 	auto map = game_.FindMap(model::Map::Id(session->GetMap()));
	auto map_speed = map->GetDogSpeed();

	player->GetDog()->SpawnDogInMap(game_.GetSpawnInRandomPoint());
	model::DogDirection dir =  json_loader::GetMoveDirection(body);
	player->GetDog()->SetSpeed(dir, map_speed > 0.0 ? map_speed : game_.GetDefaultDogSpeed());
	//std::cout << "Set Dog Parameters speed:" << (unsigned) dir << " map_speed:" <<  map_speed << "def sped: " << game_.GetDefaultDogSpeed() << std::endl;
	auto resp = MakeStringResponse(http::status::ok, "{}", http_version, keep_alive, ContentType::APPLICATION_JSON, {{http::field::cache_control, "no-cache"sv}});
	return resp;
 }

 StringResponse RequestHandler::HandleTickAction(http::verb method, std::string_view auth_type, const std::string& body, unsigned http_version, bool keep_alive)
 {
	 StringResponse resp;

	// std::cout << "Call Method HandleTickAction" << std::endl;
	  	if(method != http::verb::post)
	  	{
	  		resp = MakeStringResponse(http::status::method_not_allowed,
	  	      			              json_serializer::MakeMappedResponce({ {"code", "invalidMethod"}, {"message", "Invalid method"}}),
                                      http_version, keep_alive, ContentType::APPLICATION_JSON, {{http::field::cache_control, "no-cache"sv}});

	  		return resp;
	  	}
	  	try{
	  		int deltaTime = json_loader::ParseDeltaTimeRequest(body);
	  		//std::cout << "HandleTickAction:" << deltaTime << std::endl;
	  		game_.MoveDogs(deltaTime);
	  		resp = MakeStringResponse(http::status::ok, "{}", http_version, keep_alive, ContentType::APPLICATION_JSON, {{http::field::cache_control, "no-cache"sv}});
	  	}
	  	catch(BadDeltaTimeException& ex)
	  	{
	  		resp = MakeStringResponse(http::status::bad_request,
  				    					   json_serializer::MakeMappedResponce({ {"code", "invalidArgument"},{"message", "Failed to parse tick request JSON"}}),
 	   									   http_version, keep_alive, ContentType::APPLICATION_JSON, {{http::field::cache_control, "no-cache"sv}});
	  	}


	 return resp;
 }
*/
}  // namespace http_handler
