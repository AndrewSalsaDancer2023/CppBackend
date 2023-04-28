#include "view.h"

#include <boost/algorithm/string/trim.hpp>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <set>
#include <sstream>
#include "../app/use_cases.h"
#include "../menu/menu.h"
#include "../util/tagged_uuid.h"

using namespace std::literals;
namespace ph = std::placeholders;

namespace ui {
namespace detail {

struct BookTag {};

std::ostream& operator<<(std::ostream& out, const AuthorInfo& author) {
    out << author.name;
    return out;
}

std::ostream& operator<<(std::ostream& out, const BookInfo& book) {
    out << book.title << ", " << book.publication_year;
    return out;
}

}  // namespace detail

template <typename T>
void PrintVector(std::ostream& out, const std::vector<T>& vector) {
    int i = 1;
    for (auto& value : vector) {
        out << i++ << " " << value << std::endl;
    }
}

View::View(menu::Menu& menu, app::UseCases& use_cases, std::istream& input, std::ostream& output)
    : menu_{menu}
    , use_cases_{use_cases}
    , input_{input}
    , output_{output} {
    	menu_.AddAction(
    	"AddAuthor"s, "name"s, "Adds author"s, std::bind(&View::AddAuthor, this, ph::_1)
    	// ����
    	// [this](auto& cmd_input) { return AddAuthor(cmd_input); }
    	);
    menu_.AddAction("AddBook"s, "<pub year> <title>"s, "Adds book"s,
                    std::bind(&View::AddBook, this, ph::_1));
    menu_.AddAction("ShowAuthors"s, {}, "Show authors"s, std::bind(&View::ShowAuthors, this));
    menu_.AddAction("ShowBooks"s, {}, "Show books"s, std::bind(&View::ShowBooks, this));
    menu_.AddAction("ShowAuthorBooks"s, {}, "Show author books"s,
                    std::bind(&View::ShowAuthorBooks, this));
    menu_.AddAction("DeleteAuthor"s, "name"s, "Deletes author"s, std::bind(&View::DeleteAuthor, this, ph::_1)
    );
}

void View::AddAuthorByName(std::string& name) const
{
    boost::algorithm::trim(name);
    if(name.empty())
    	throw std::exception();
    use_cases_.AddAuthor(std::move(name));
}

bool View::AddAuthor(std::istream& cmd_input) const {
    try {
        std::string name;
        std::getline(cmd_input, name);
        AddAuthorByName(name);
    } catch (const std::exception&) {
        output_ << "Failed to add author"sv << std::endl;
    }
    return true;
}

bool View::DeleteAuthor(std::istream& cmd_input) const
{
	std::string author_name;
try {
	cmd_input >> author_name;
   //	    std::getline(cmd_input, params.title);
	boost::algorithm::trim(author_name);

	 if(author_name.empty())
	 {
		auto author_id = SelectAuthor();
		if (not author_id.has_value())
			throw std::exception();
		else
		{

		}
	 }
	 else
	 {
		 const auto& authors = GetAuthors();
		 auto itFind = std::find_if(authors.begin(), authors.end(), [&author_name](auto& info)
		       	    			{
		       	    				return info.name == author_name;
		       	    			});
		 if(itFind == authors.end())
			 throw std::exception();

		 use_cases_.DeleteAuthor(*itFind);
/*
	    else {
	        params.author_id = author_id.value();
	        auto uuid = util::TaggedUUID<ui::detail::BookTag>::New();
	        params.id = uuid.ToString();
	    }*/
	 }
}    catch (const std::exception&) {
	           output_ << "Failed to delete author"sv << std::endl;
	       }

	return true;
}


std::string NormalizeTag(std::string& word)
{
    std::string rs;
    std::vector<std::string> res;

    std::istringstream iss(word);
    while (std::getline(iss, word, ' ')) {
        boost::algorithm::trim(word);
        if(word.empty())
            continue;
        res.push_back(word);
    }

    for(auto it = res.begin(); it != res.end(); ++it)
        rs = rs + *it + " ";

     rs.erase(rs.size()-1) ;
     return rs;
}

std::string NormalizeTags(const std::string& sentence)
{
    std::string word;
    std::set<std::string> tags;

    std::istringstream iss(sentence);
    while (std::getline(iss, word, ',')) {
        boost::algorithm::trim(word);
        if(word.empty())
            continue;
        word = NormalizeTag(word);
        tags.insert(word);
    }
    word.clear();
    for(auto it = tags.begin(); it != tags.end(); ++it)
        word = word + *it + ", ";

     word.erase(word.size()-2) ;
     return word;
}

std::string View::GetTags() const
{
	//add tags
	output_ << "Enter tags (comma separated):" << std::endl;
	std::string tags;
	if (!std::getline(input_, tags) || tags.empty())
		tags.clear();
	else
	{
		boost::algorithm::trim(tags);
		tags = NormalizeTags(tags);
	}

	return tags;
}

bool View::AddBook(std::istream& cmd_input) const {
    try {
        if (auto params = GetBookParams(cmd_input)) {

        	output_ << "Enter author name or empty line to select from list:" << std::endl;
       	    std::string author;
       	    if (!std::getline(input_, author) || author.empty()) {
       	    	//select from list
       	    	ShowAuthors();
       	    	auto author = SelectAuthor();
       	    	if(author)
       	    		(*params).author_id = *author;
       	    	else
       	    		throw std::exception();

    	    	 (*params).tags = GetTags();
    	         //use_cases_.AddBook(*params);
       	    }
       	    else
       	    {
       	    	//author in command line
       	    	boost::algorithm::trim(author);
       	    	const auto& authors = GetAuthors();
       	    	auto itFind = std::find_if(authors.begin(), authors.end(), [&author](auto& info)
       	    			{
       	    				return info.name == author;
       	    			});
       	    	if(itFind != authors.end())
       	    	{
       	    		//add author
       	    		(*params).author_id = itFind->id;
       	    	}
       	    	else
       	    	{
       	    		std::string addAuthPrompt = "No author found. Do you want to add " + author +" (y/n)?";
       	    		output_ << addAuthPrompt << std::endl;
       	    		std::string answer;
       	    		if (!std::getline(input_, answer) || (answer!= "Y" && answer!= "y"))
       	    			throw std::exception();
       	    		else
       	    		{
       	    			AddAuthorByName(author);
       	    			const auto& authors = GetAuthors();
       	    			 auto itFind = std::find_if(authors.begin(), authors.end(), [&author](auto& info)
       	    			   	    			{
       	    			   	    				return info.name == author;
       	    			   	    			});
       	    			if(itFind == authors.end())
       	    				throw std::exception();
       	    			(*params).author_id = itFind->id;
       	    		}
       	    	}
	    		(*params).tags = GetTags();
       	    }
       	    std::cout << " id: " << (*params).id << " title: " << (*params).title << " author_id: "
       	    		<< (*params).author_id << " tags:" << (*params).tags << " year: " << (*params).publication_year;
       	 //use_cases_.AddBook(*params);
        }
    } catch (const std::exception&) {
        output_ << "Failed to add book"sv << std::endl;
    }
    return true;
}

bool View::ShowAuthors() const {
    PrintVector(output_, GetAuthors());
    return true;
}

bool View::ShowBooks() const {
    PrintVector(output_, GetBooks());
    return true;
}

bool View::ShowAuthorBooks() const {
    // TODO: handle error
    try {
        if (auto author_id = SelectAuthor()) {
            PrintVector(output_, GetAuthorBooks(*author_id));
        }
    } catch (const std::exception&) {
        throw std::runtime_error("Failed to Show Books");
    }
    return true;
}

std::optional<detail::AddBookParams> View::GetBookParams(std::istream& cmd_input) const {
    detail::AddBookParams params;

    cmd_input >> params.publication_year;
    std::getline(cmd_input, params.title);
    boost::algorithm::trim(params.title);

    if(params.title.empty() || !params.publication_year)
           	throw std::exception();

    auto author_id = SelectAuthor();
    if (not author_id.has_value())
        return std::nullopt;
    else {
        params.author_id = author_id.value();
        auto uuid = util::TaggedUUID<ui::detail::BookTag>::New();
        params.id = uuid.ToString();
        return params;
    }
}

std::optional<std::string> View::SelectAuthor() const {
    output_ << "Select author:" << std::endl;
    auto authors = GetAuthors();
    PrintVector(output_, authors);
    output_ << "Enter author # or empty line to cancel" << std::endl;

    std::string str;
    if (!std::getline(input_, str) || str.empty()) {
        return std::nullopt;
    }

    int author_idx;
    try {
        author_idx = std::stoi(str);
    } catch (std::exception const&) {
        throw std::runtime_error("Invalid author num");
    }

    --author_idx;
    if (author_idx < 0 or author_idx >= authors.size()) {
        throw std::runtime_error("Invalid author num");
    }

    return authors[author_idx].id;
}

std::vector<detail::AuthorInfo> View::GetAuthors() const {
    std::vector<detail::AuthorInfo> dst_autors;
    //assert(!"TODO: implement GetAuthors()");
    dst_autors = use_cases_.GetAuthors();
    return dst_autors;
}

std::vector<detail::BookInfo> View::GetBooks() const {
    std::vector<detail::BookInfo> books;
    books = use_cases_.GetBooks();
    return books;
}

std::vector<detail::BookInfo> View::GetAuthorBooks(const std::string& author_id) const {
    std::vector<detail::BookInfo> books;
//    assert(!"TODO: implement GetAuthorBooks()");
    books = use_cases_.GetAuthorBooks(author_id);
    return books;
}

}  // namespace ui
