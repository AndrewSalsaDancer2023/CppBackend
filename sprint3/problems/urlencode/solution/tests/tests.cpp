#include <gtest/gtest.h>

#include "../src/urlencode.h"

using namespace std::literals;

TEST(UrlEncodeTestSuite, OrdinaryCharsAreNotEncoded) {
    EXPECT_EQ(UrlEncode(""sv), ""s);
    EXPECT_EQ(UrlEncode("hello"sv), "hello"s);
}

TEST(UrlEncodeTestSuite, SpecialCharsAreEncoded) {
    EXPECT_EQ(UrlEncode("hello"sv), "hello"s);
    EXPECT_EQ(UrlEncode("Hello World!"sv), "Hello+World%21"s);    
    EXPECT_EQ(UrlEncode("abc*"sv), "abc%2a"s);    
}

TEST(UrlEncodeTestSuite, WhitespacesAreEncoded) {
    EXPECT_EQ(UrlEncode(" "sv), "+"s);
    EXPECT_EQ(UrlEncode("  "sv), "++"s);    
    EXPECT_EQ(UrlEncode("   "sv), "+++"s);    
}

/* Напишите остальные тесты самостоятельно */
