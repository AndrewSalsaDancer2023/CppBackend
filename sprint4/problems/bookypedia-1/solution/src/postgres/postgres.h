#pragma once
#include <pqxx/connection>
#include <pqxx/transaction>

#include "../domain/author.h"

namespace postgres {

class AuthorRepositoryImpl : public domain::AuthorRepository {
public:
    explicit AuthorRepositoryImpl(pqxx::connection& connection)
        : connection_{connection} {
    }

    void Save(const domain::Author& author) override;
    std::vector<ui::detail::AuthorInfo> Load() override;
    void AddBook(const ui::detail::AddBookParams& params) override;
    virtual std::vector<ui::detail::BookInfo> GetBooks() override;
    std::vector<ui::detail::BookInfo> GetAuthorBooks(const std::string& author_id) override;
private:
    pqxx::connection& connection_;
};

class Database {
public:
    explicit Database(pqxx::connection connection);

    AuthorRepositoryImpl& GetAuthors() & {
        return authors_;
    }

private:
    pqxx::connection connection_;
    AuthorRepositoryImpl authors_{connection_};
};

}  // namespace postgres
