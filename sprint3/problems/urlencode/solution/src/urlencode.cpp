#include "urlencode.h"
#include <sstream>

bool isIncorrectSymbol(char symb)
{
    std::string incor_symb = "!#$&'()*+,/:;=?@[]";
    return (incor_symb.find(symb) != std::string::npos);
}

std::string encodeSymbol(char symb)
{
    std::ostringstream ss;
    int smb = static_cast<int>(symb);
    ss << "%" << std::hex << smb;

    return ss.str();
}

std::string UrlEncode(std::string_view str) {
    std::string result;

    //size_t last_index = str.size() - 1;
    for(size_t i = 0; i < str.size(); ++i)
    {
        if(str[i] == ' ')
            result += '+';
        else
          if(isIncorrectSymbol(str[i]) || (str[i] < 32) || (str[i] >= 128))
              result += encodeSymbol(str[i]);
           else
              result += str[i];
    }

    return result;
}

