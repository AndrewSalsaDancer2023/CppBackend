#include "use_cases_impl.h"

#include "../domain/author.h"

namespace app {
using namespace domain;

void UseCasesImpl::AddAuthor(const std::string& name) {
    authors_.Save({AuthorId::New(), name});
}

std::vector<ui::detail::AuthorInfo> UseCasesImpl::GetAuthors()
{
	std::vector<ui::detail::AuthorInfo> res  = authors_.Load();
	return res;
}

void UseCasesImpl::DeleteAuthor(const ui::detail::AuthorInfo& auth_info)
{
	authors_.Delete(auth_info);
}

void UseCasesImpl::AddBook(const ui::detail::AddBookParams& params)
{
	authors_.AddBook(params);
}

std::vector<ui::detail::BookInfo> UseCasesImpl::GetBooks()
{
	std::vector<ui::detail::BookInfo> res = authors_.GetBooks();
	return res;
}

std::vector<ui::detail::BookInfo> UseCasesImpl::GetAuthorBooks(const std::string& author_id)
{
	std::vector<ui::detail::BookInfo> res = authors_.GetAuthorBooks(author_id);
	return res;
}
}  // namespace app
