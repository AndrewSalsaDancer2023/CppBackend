#pragma once
#include <string>

#include "../util/tagged_uuid.h"
#include "../ui/view.h"
namespace domain {

namespace detail {
struct AuthorTag {};
}  // namespace detail

using AuthorId = util::TaggedUUID<detail::AuthorTag>;

class Author {
public:
    Author(AuthorId id, std::string name)
        : id_(std::move(id))
        , name_(std::move(name)) {
    }

    const AuthorId& GetId() const noexcept {
        return id_;
    }

    const std::string& GetName() const noexcept {
        return name_;
    }

private:
    AuthorId id_;
    std::string name_;
};

class AuthorRepository {
public:
    virtual void Save(const Author& author) = 0;
    virtual void Delete(const ui::detail::AuthorInfo& auth_info) = 0;
    virtual std::vector<ui::detail::AuthorInfo> Load() = 0;
    virtual void AddBook(const ui::detail::AddBookParams& params) = 0;
    virtual std::vector<ui::detail::BookInfo> GetBooks() = 0;
    virtual std::vector<ui::detail::BookInfo> GetAuthorBooks(const std::string& author_id) = 0;
protected:
    ~AuthorRepository() = default;
};

}  // namespace domain
