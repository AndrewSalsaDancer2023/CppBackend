#include "request_handler.h"
#include <boost/algorithm/string.hpp>    
#include <boost/beast.hpp>
#include <iostream>
#include <string_view>

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

std::string GetMimeType(std::string_view extension)
{
  auto it = extensionToMime.find(extension);
  if(it != extensionToMime.end())	
	return it->second;
   return "";
}

std::string MakePathLowerCased(const std::string& str)
{
	return boost::to_lower_copy(str);
}

bool IsSubPath(fs::path path, fs::path base) {
    // Приводим оба пути к каноничному виду (без . и ..)
    path = fs::weakly_canonical(path);
    base = fs::weakly_canonical(base);

    // Проверяем, что все компоненты base содержатся внутри path
    for (auto b = base.begin(), p = path.begin(); b != base.end(); ++b, ++p) {
        if (p == path.end() || *p != *b) {
            return false;
        }
    }
    return true;
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

StringResponse RequestHandler::MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
                                                   bool keep_alive, std::string_view content_type,
												   const std::initializer_list< std::pair<http::field, std::string_view> > & addition_headers)
 {
    StringResponse response(status, http_version);
    response.set(http::field::content_type, content_type);
//    if(cache_control.size())
 //   	response.set(http::field::cache_control, cache_control);
 //   if(allow_headers.size())
 //   	response.set(http::field::allow, allow_headers);
    //std::map<http::field, std::string_view> headers(addition_headers);
    for(auto it = addition_headers.begin(); it != addition_headers.end(); ++it)
    	response.set(it->first, it->second);

    response.body() = body;
    response.content_length(body.size());
    response.keep_alive(keep_alive);
    return response;
 }

 http::response<http::file_body> PrepareFile(const std::filesystem::path& json_path, const std::string& mime_type)
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
 
 
}  // namespace http_handler
