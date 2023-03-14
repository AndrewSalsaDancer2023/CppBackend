#include <exception>  

using namespace std;  
class MapNotFoundException : public exception
{  
    public:  
        const char * what() const throw()  
        {  
            return "Map not found\n";  
        }  
};  

class EmptyNameException : public exception
{  
    public:  
        const char * what() const throw()  
        {  
            return "Empty player name specified\n";  
        }  
};  

class ParsingJsonException : public exception
{  
    public:  
        const char * what() const throw()  
        {  
            return "Invalid Json\n";  
        }  
};  

class MetodNotAllowedException : public exception
{  
    public:  
        const char * what() const throw()  
        {  
            return "Invalid HTTP method\n";  
        }  
};  

