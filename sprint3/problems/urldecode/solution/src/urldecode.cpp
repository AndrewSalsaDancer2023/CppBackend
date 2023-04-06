#include "urldecode.h"

#include <charconv>
#include <stdexcept>
#include <sstream>

bool isIncorrectSymbol(char symb)
{
    std::string incor_symb = "!#$&'()*+,/:;=?@[]";
    return (incor_symb.find(symb) != std::string::npos);
}

char decodeSymbol(std::string src)
{
    unsigned int x;
     std::stringstream ss;
     ss << std::hex << src;
     ss >> x;

     char res = static_cast<char>(x);
    return res;
}

bool isAlphabetSymbol(size_t symb)
{
    if ( ((symb >= 'a') && (symb <= 'z')) ||
         ((symb >= 'A') && (symb <= 'Z')) )
        return true;

    return false;
}

bool isDigit(size_t symb)
{
    if((symb >= '0') && (symb <= '9'))
        return true;
    return false;
}



std::string UrlDecode(std::string_view str) {
    std::string result;

    size_t last_index = str.size() - 1;
    for(size_t i = 0; i < str.size(); )
    {
        if(isAlphabetSymbol(str[i]) || isDigit(str[i]))
        {
           result += str[i];
           ++i;
        }
        else
            if(str[i] == '+')
            {
                result += ' ';
                ++i;
            }
        else
         {
           if(str[i] == '%')
           {
               if( ((i+1) >=last_index) || ((i+2) >last_index) ||
                    !isDigit(str[i+1]) || !isDigit(str[i+2]) ||
                     isIncorrectSymbol(str[i+1]) || isIncorrectSymbol(str[i+2]))
                   throw std::invalid_argument("Invalid encoded symbol");
               else
               {
                   std::string src;
                   src += str[i+1];
                   src += str[i+2];
                   result += decodeSymbol(src);
                   i+=3;
               }
           }
            else
           {
              result += str[i];
              ++i;
           }
        }
    }
    return result;
}
