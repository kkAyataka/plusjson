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
