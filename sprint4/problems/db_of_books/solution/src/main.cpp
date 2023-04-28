// main.cpp
#include <string>
#include <sstream>
#include <iostream>
#include <boost/json.hpp>
#include <pqxx/pqxx>
#include <pqxx/except>
#include <optional>

using namespace std::literals;
// libpqxx использует zero-terminated символьные литералы вроде "abc"_zv;
using pqxx::operator"" _zv;
namespace json = boost::json;

constexpr auto tag_add_book = "add_book"_zv;
constexpr auto tag_all_books = "all_books"_zv;

std::string last_command;
struct book_info {
	std::string title;
	std::string author;
	int year;
	std::optional<std::string> ISBN;
};

book_info parse_book_info(const json::object& obj)
{
	book_info res;
	res.title = obj.at("title").as_string().c_str();
	res.author = obj.at("author").as_string().c_str();
	res.year = obj.at("year").as_int64();
	if(obj.contains("ISBN") && !obj.at("ISBN").is_null())	
		res.ISBN = obj.at("ISBN").as_string().c_str();
		
	return res;
}

std::string MakeResultResponce(bool result)
{
	json::object resp_object;
        resp_object["result"] = result;

      	return json::serialize(resp_object);
}

json::string execute_request(const std::string& json_string, pqxx::connection& conn)
{
	json::string result;
	auto value = json::parse(json_string);
   	
        json::object object_to_read = value.as_object();
        last_command = "";
        
        if(object_to_read.contains("action"))
	{
		auto action = object_to_read.at("action").as_string();
		if(action == "add_book")
		{
			auto payload = object_to_read.at("payload").as_object();
			book_info res = parse_book_info(payload);

			pqxx::work w(conn);

			if(res.ISBN)
				w.exec_prepared(tag_add_book, res.title, res.author, res.year, *res.ISBN);
			else
				w.exec_prepared(tag_add_book, res.title, res.author, res.year, nullptr);
			w.commit();
			
			std::cout << MakeResultResponce(true) << std::endl;
		        last_command = "add_book";
		}
		else
		if(action == "all_books")
		{
			pqxx::read_transaction rd(conn);
			auto query_text = "SELECT * FROM books ORDER BY year DESC, title, author, ISBN;"_zv;
			json::array resp_object;
		    // Выполняем запрос и итерируемся по строкам ответа
    		   for (auto [id, title, author, year, ISBN] : rd.query<int, std::string_view, std::string_view, int, std::optional<std::string>>(query_text))
    		   {	        	
	        	json::object book_object;
   		        	
		        book_object["id"] = id;
			book_object["title"] = std::string(title);
			book_object["author"] = std::string(author);
			book_object["year"] = year;
			if(ISBN)
				book_object["ISBN"] = *ISBN;
			else
				book_object["ISBN"] = nullptr;
				
	        	resp_object.emplace_back(book_object);
		    }
		    std::cout << json::serialize(resp_object) << std::endl;
		}

		result = action;
	}
	
	return result;
}

void prepare_table(pqxx::connection& conn)
{
	pqxx::work w(conn);
        w.exec("CREATE TABLE IF NOT EXISTS books (id SERIAL PRIMARY KEY, title varchar(100) NOT NULL, author varchar(100) NOT NULL, year integer NOT NULL, ISBN char(13) UNIQUE);"_zv);

      	w.exec("DELETE FROM books;"_zv);
      	
        w.commit();
}

int main(int argc, const char* argv[]) {

        if (argc == 1) {
            std::cout << "Usage: db_example <conn-string>\n"sv;
            return EXIT_SUCCESS;
        } else if (argc != 2) {
            std::cerr << "Invalid command line\n"sv;
            return EXIT_FAILURE;
        }

        // Подключаемся к БД, указывая её параметры в качестве аргумента
        pqxx::connection conn{argv[1]};
	prepare_table(conn);
	
	conn.prepare(tag_add_book, "INSERT INTO books (title, author, year, ISBN) VALUES ($1, $2, $3, $4);"_zv); 

    	do{
    	try{
		std::string json_string;
		std::getline(std::cin, json_string);
	    	
	    	auto action = execute_request(json_string, conn);
	    	if(action == "exit")
			return EXIT_SUCCESS;
		    	
       	std::cin >> std::ws;
         }
         catch(const pqxx::sql_error &ex)
	{
	  const pqxx::sql_error *s=dynamic_cast<const pqxx::sql_error*>(&ex);
	  if(s && (s->query() == "add_book")) 
	  {
		  std::cout << MakeResultResponce(false) << std::endl;
	  }
	}

    	}while(true);
    
    return EXIT_SUCCESS;
}
