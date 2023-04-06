#define BOOST_TEST_MODULE urlencode tests
#include <boost/test/unit_test.hpp>

#include "../src/urldecode.h"

BOOST_AUTO_TEST_CASE(UrlDecode_tests) {
    using namespace std::literals;

    BOOST_TEST(UrlDecode(""sv) == ""s);
    BOOST_TEST(UrlDecode("Hello World !"sv) == "Hello World !"s);
    BOOST_TEST(UrlDecode("Hello%20World !"sv) == "Hello World !"s);
    
    BOOST_TEST(UrlDecode("Hello+World%20%21"sv) == "Hello World !"s);
    BOOST_TEST(UrlDecode("Hello+World%20"sv) == "Hello World "s);
        
    BOOST_TEST(UrlDecode("+"sv) == " "s);        
    // Напишите остальные тесты для функции UrlDecode самостоятельно
}

BOOST_AUTO_TEST_CASE(UrlThrow_tests) {
    using namespace std::literals;
    BOOST_CHECK_THROW(UrlDecode("Hello+World%2"sv), std::invalid_argument);
    BOOST_CHECK_THROW(UrlDecode("Hello+World%"sv), std::invalid_argument);
    BOOST_CHECK_THROW(UrlDecode("Hello+World%2 "sv), std::invalid_argument);
    BOOST_CHECK_THROW(UrlDecode("Hello+World%!"sv), std::invalid_argument);
    BOOST_CHECK_THROW(UrlDecode("Hello+World%@"sv), std::invalid_argument);
}
