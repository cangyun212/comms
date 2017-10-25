
#include "coretest.hpp"

#include <limits>
#include <sstream>

#include "Concatenate.hpp"

using namespace sg;

TEST(Concatenate, StringString)
{
    std::string a;
    concatenate(a, "A", " ", "small", " ", "string");
    EXPECT_EQ(std::string("A small string"), a);

    std::string b;
    concatenate(b, "A", " ", "very very very very very very very very very very very very very very very long", " string");
    EXPECT_EQ(std::string("A very very very very very very very very very very very very very very very long string"), b);

    std::string c("c is ");
    std::string str("string");
    concatenate(c, "a ", std::string("std::"), str, "");
    EXPECT_EQ(std::string("c is a std::string"), c);

    std::string d("d is a");
    char strarr[] = { 's', 't', 'r', 'i', 'n', 'g', 0 };
    concatenate(d, std::string(" "), strarr, " array");
    EXPECT_EQ(std::string("d is a string array"), d);

    std::string e;
    concatenate(e, "", std::string());
    std::string emptyStr;
    EXPECT_EQ(emptyStr, e);
}

TEST(Concatenate, StringInteger)
{
    std::string a;
    concatenate(a, "Number A is ", 128, std::string(", not "), 0);
    EXPECT_EQ(std::string("Number A is 128, not 0"), a);

    std::string b("String b");
    int i = 101;
    std::string str(" contains");
    concatenate(b, str, " ", "number ", i);
    EXPECT_EQ(std::string("String b contains number 101"), b);

    std::string c;
    char strarr[] = { 'T', 'h', 'e', '\0' };
    concatenate(c, strarr, std::string(" minimum number of int32_t is "), std::numeric_limits<int32_t>::min(), ".");
    EXPECT_EQ(std::string("The minimum number of int32_t is -2147483648."), c);

    std::string d("The");
    int64_t min64 = std::numeric_limits<int64_t>::min();
    concatenate(d, " min number of uint32_t is ", 0, ", and the max number of uint32_t is ",
        std::numeric_limits<uint32_t>::max(), std::string(". The min number of int64_t is "), min64);
    std::string expect(
        "The min number of uint32_t is 0, and the max number of uint32_t is 4294967295. The min number of int64_t is -9223372036854775808");
    EXPECT_EQ(expect, d);
}

TEST(Concatenate, StringFloat)
{
    std::string a;
    concatenate(a, "The float number is ", 15.234f, std::string("f"));
    EXPECT_EQ(std::string("The float number is 15.234f"), a);

    std::string b("b is a string");
    std::string str("double");
    concatenate(b, " that the min number of ", str, std::string(" is "), std::numeric_limits<double>::min(), ", and the max number is ",
        std::numeric_limits<double>::max());

    std::ostringstream oss;
    oss << "b is a string that the min number of double is 0.0"
        << ", and the max number is 1.7976931348623157e308"; //<< std::setprecision(17) << std::numeric_limits<double>::max();
    EXPECT_EQ(oss.str(), b);

    std::string c("c is a string");
    float f = 0.578f;
    double d = 133.0308;
    concatenate(c, " contains f ", f, " and d ", d);
    std::ostringstream oss2;
    oss2 << "c is a string contains f " 
        << std::setprecision(4) << f 
        << " and d " << std::setprecision(7) << d;
    //EXPECT_EQ(std::string("c is a string contains f 0.579000 and d 133.0308"), c);
    EXPECT_EQ(oss2.str(), c);
}

TEST(Concatenate, StringIntegerFloat)
{
    std::string a;
    int i = 0;
    concatenate(a, "a string concatenate integer ", i, std::string(" and float number "), 58.94992f);
    EXPECT_EQ(std::string("a string concatenate integer 0 and float number 58.94992"), a);

    std::string b("b string");
    float f = 153.0f;
    double d = 0.00575;
    concatenate(b, " concatenate float number ", f, std::string(" and double number "), d, " with integer number ", 7790);

    std::ostringstream oss;
    oss << "b string concatenate float number 153.0"
        << " and double number " << std::setprecision(6) << d
        << " with integer number 7790";
    //EXPECT_EQ(std::string("b string concatenate float number 156.0 and double number 0.0057 with integer number 7790"), b);
    EXPECT_EQ(oss.str(), b);
}

