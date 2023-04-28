#include "postgres.h"
#include <string_view>
#include <string>
//#include <pqxx/zview.hxx>
#include <pqxx/pqxx>
#include <pqxx/except>
#include "../domain/author.h"

namespace postgres {

using namespace std::literals;
using pqxx::operator"" _zv;

void AuthorRepositoryImpl::Save(const domain::Author& author) {
    // Пока каждое обращение к репозиторию выполняется внутри отдельной транзакции
    // В будущих уроках вы узнаете про паттерн Unit of Work, при помощи которого сможете несколько
    // запросов выполнить в рамках одной транзакции.
    // Вы также может самостоятельно почитать информацию про этот паттерн и применить его здесь.
    pqxx::work work{connection_};
    work.exec_params(
        R"(
INSERT INTO authors (id, name) VALUES ($1, $2)
ON CONFLICT (id) DO UPDATE SET name=$2;
)"_zv,
        author.GetId().ToString(), author.GetName());
    work.commit();
}

void AuthorRepositoryImpl::Delete(const ui::detail::AuthorInfo& author)
{
    pqxx::work work{connection_};
    work.exec_params(
        R"(DELETE FROM authors WHERE authors.id = $1)"_zv, author.id);

    work.exec_params(
            R"(DELETE FROM books, book_tags WHERE books.author_id = $1 AND books.id = book_tags.book_id)"_zv, author.id);

    work.commit();
}


std::vector<ui::detail::AuthorInfo> AuthorRepositoryImpl::Load() {

	pqxx::read_transaction rd(connection_);
	auto query_text = "SELECT id, name FROM authors ORDER BY name ASC"_zv;

	std::vector<ui::detail::AuthorInfo> res;
	 // Выполняем запрос и итерируемся по строкам ответа
	   for (auto [id, name] : rd.query<std::string, std::string>(query_text))
	   {
		   ui::detail::AuthorInfo author{id, name};
		   res.push_back(author);
	   }

	return res;
}

void AuthorRepositoryImpl::AddBook(const ui::detail::AddBookParams& params) {
    pqxx::work work{connection_};
    work.exec_params(
        R"(
INSERT INTO books (id, author_id, title, publication_year) VALUES ($1, $2, $3, $4);
)"_zv,
	params.id, params.author_id, params.title,  params.publication_year);

    work.exec_params(
        R"(INSERT INTO book_tags (book_id, tag) VALUES ($1, $2);)"_zv, params.id, params.tags);

    work.commit();
}

std::vector<ui::detail::BookInfo> AuthorRepositoryImpl::GetBooks()
{
	pqxx::read_transaction rd(connection_);
	auto query_text = "SELECT title, publication_year FROM books ORDER BY title ASC"_zv;

	std::vector<ui::detail::BookInfo> res;
	 // Выполняем запрос и итерируемся по строкам ответа
	 for (auto [title, publication_year] : rd.query<std::string, int>(query_text))
	 {
	   ui::detail::BookInfo book{title, publication_year};
	   res.push_back(book);
	 }

	return res;
}

std::vector<ui::detail::BookInfo> AuthorRepositoryImpl::GetAuthorBooks(const std::string& author_id)
{
	pqxx::read_transaction rd(connection_);
    auto query_text = "SELECT title, publication_year FROM books WHERE author_id = "+rd.quote(author_id);
	std::vector<ui::detail::BookInfo> res;
	// Выполняем запрос и итерируемся по строкам ответа
	 for (auto [title, publication_year] : rd.query<std::string, int>(query_text))
	 {
	   ui::detail::BookInfo book{title, publication_year};
	   res.push_back(book);
	 }

	return res;
}

Database::Database(pqxx::connection connection)
    : connection_{std::move(connection)} {
    pqxx::work work{connection_};
    work.exec(R"(
CREATE TABLE IF NOT EXISTS authors (
    id UUID CONSTRAINT author_id_constraint PRIMARY KEY,
    name varchar(100) UNIQUE NOT NULL
);
)"_zv);
    // ... создать другие таблицы

    work.exec(R"(
CREATE TABLE IF NOT EXISTS books (
    id UUID CONSTRAINT book_id_constraint PRIMARY KEY,
	author_id UUID NOT NULL,
    title varchar(100) NOT NULL,
	publication_year integer NOT NULL
);
)"_zv);

    work.exec(R"(
CREATE TABLE IF NOT EXISTS book_tags (
    book_id UUID CONSTRAINT book_tags_id_constraint PRIMARY KEY,
    tag varchar(30)
);
)"_zv);

    // коммитим изменения
    work.commit();
}

}  // namespace postgres
