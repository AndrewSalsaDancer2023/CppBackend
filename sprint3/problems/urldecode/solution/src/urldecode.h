#pragma once

#include <string>

bool isIncorrectSymbol(char symb);
char decodeSymbol(std::string src);
bool isAlphabetSymbol(size_t symb);
bool isDigit(size_t symb);

/*
Возвращает URL-декодированное представление строки str.
Пример: "Hello+World%20%21" должна превратиться в "Hello World !"
В случае ошибки выбрасывает исключение std::invalid_argument
*/
std::string UrlDecode(std::string_view str);
