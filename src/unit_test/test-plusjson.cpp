#include "plusjson/plusjson.hpp"

#include "gtest/gtest.h"

// Constructor, is(), get_type()

TEST(JSON, value__object) {
    plusjson::Value v((plusjson::Object()));

    ASSERT_EQ(v.get_type(), plusjson::TYPE_OBJECT);
    ASSERT_EQ(v.is<plusjson::Object>(), true);
}

TEST(JSON, value__array) {
    plusjson::Value v((plusjson::Array()));

    ASSERT_EQ(v.get_type(), plusjson::TYPE_ARRAY);
    ASSERT_EQ(v.is<plusjson::Array>(), true);
}

TEST(JSON, value__number) {
    plusjson::Value v(1.0);

    ASSERT_EQ(v.get_type(), plusjson::TYPE_NUMBER);
    ASSERT_EQ(v.is<plusjson::Number>(), true);
}

TEST(JSON, value__string) {
    plusjson::Value v(std::string("string"));

    ASSERT_EQ(v.get_type(), plusjson::TYPE_STRING);
    ASSERT_EQ(v.is<plusjson::String>(), true);
}

TEST(JSON, value__string_char) {
    plusjson::Value v("string");

    ASSERT_EQ(v.get_type(), plusjson::TYPE_STRING);
    ASSERT_EQ(v.is<plusjson::String>(), true);
}

TEST(JSON, value__boolean) {
    plusjson::Value v(true);

    ASSERT_EQ(v.get_type(), plusjson::TYPE_BOOLEAN);
    ASSERT_EQ(v.is<plusjson::Boolean>(), true);
}

TEST(JSON, value) {
    plusjson::Value v;

    ASSERT_EQ(v.is<plusjson::Null>(), true);
    ASSERT_EQ(v.get_type(), plusjson::TYPE_NULL);
}

TEST(JSON, value__value) {
    plusjson::Value v1(1.1);
    plusjson::Value v2(v1);

    ASSERT_EQ(v2.get_type(), plusjson::TYPE_NUMBER);
    ASSERT_EQ(v2.get<plusjson::Number>(), v1.get<plusjson::Number>());
    ASSERT_EQ(v2, v1);

    v1.get<plusjson::Number>() = 2;
    ASSERT_NE(v2, v1);
}

// Copy constructor, =, == and != operator

namespace {

template<class T>
void test_copy_and_eq(const T raw1, const T raw2) {
    plusjson::Value v1(raw1);

    plusjson::Value v2(v1);
    plusjson::Value v3;
    v3 = v2;

    ASSERT_EQ(v1, v2);
    ASSERT_EQ(v1, v3);

    v1.get<T>() = raw2;

    ASSERT_NE(v1, v2);
    ASSERT_NE(v1, v3);
}

} // no namespace

TEST(JSON, copy_and_ep_object) {
    plusjson::Object o1;
    o1["key"] = plusjson::Value("Value");

    plusjson::Object o2;
    o2["key1"] = plusjson::Value("Value1");

    test_copy_and_eq(o1, o2);
}

TEST(JSON, copy_and_eq_array) {
    plusjson::Array a1;
    a1.push_back(plusjson::Value(1.0));

    plusjson::Array a2;

    test_copy_and_eq(a1, a2);
}

TEST(JSON, copy_and_ep_number) {
    test_copy_and_eq(123.4, -1.1);
}

TEST(JSON, copy_and_ep_string) {
    const plusjson::String s1 = "StringA";
    const plusjson::String s2 = "I'm lovin' it";

    test_copy_and_eq(s1, s2);
}

TEST(JSON, copy_and_eq_boolean) {
    test_copy_and_eq(true, false);
}

TEST(JSON, copy_and_eq_null) {
    const plusjson::Value v1;
    const plusjson::Value v2;
    const plusjson::Value v3(1.0);

    ASSERT_EQ(v1, v2);
    ASSERT_NE(v1, v3);
}

// get

TEST(JSON, get_invalid_type) {
    plusjson::Object o;
    o["k"] = plusjson::Value(1.0);

    const plusjson::Value v(o);

    ASSERT_EQ(v.get<plusjson::Object>(), o);
    ASSERT_THROW(v.get<plusjson::Number>(), std::bad_cast);
}
