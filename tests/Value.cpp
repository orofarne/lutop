#include <gtest/gtest.h>

#include <iostream>

#include "Value.hpp"

using namespace lutop;

TEST(Value, Nil) {
    Value v;
    EXPECT_EQ(Value::Type::Nil, v.type());

    Value v2(Value::Type::Nil);
    EXPECT_EQ(Value::Type::Nil, v2.type());

    Value v3(Value::Type::Nil, 10);
    EXPECT_EQ(Value::Type::Nil, v3.type());

    EXPECT_THROW(v.as<double>(), Value::invalid_cast);
}

TEST(Value, Boolean) {
    Value v;
    EXPECT_NE(Value::Type::Boolean, v.type());

    Value v2(Value::Type::Boolean);
    EXPECT_EQ(Value::Type::Boolean, v2.type());
    EXPECT_FALSE(v2.as<bool>());

    Value v3(Value::Type::Boolean, true);
    EXPECT_EQ(Value::Type::Boolean, v3.type());
    EXPECT_TRUE(v3.as<bool>());

    Value v4(Value::Type::Boolean, 10);
    EXPECT_EQ(Value::Type::Boolean, v4.type());
    EXPECT_TRUE(v4.as<bool>());
}

TEST(Value, Numeric) {
    Value v;
    EXPECT_NE(Value::Type::Numeric, v.type());

    Value v2(Value::Type::Numeric);
    EXPECT_EQ(Value::Type::Numeric, v2.type());
    EXPECT_DOUBLE_EQ(0., v2.as<double>());

    Value v3(Value::Type::Numeric, 3.14);
    EXPECT_EQ(Value::Type::Numeric, v3.type());
    EXPECT_DOUBLE_EQ(3.14, v3.as<double>());

    Value v4(Value::Type::Numeric, 100005);
    EXPECT_EQ(Value::Type::Numeric, v4.type());
    EXPECT_DOUBLE_EQ(100005, v4.as<double>());

    EXPECT_TRUE(v4.as<bool>());

    EXPECT_EQ(0, v4.as<std::string>().compare(0, 6, "100005", 6));
}

TEST(Value, String) {
    Value v;
    EXPECT_NE(Value::Type::String, v.type());

    Value v2(Value::Type::String);
    EXPECT_EQ(Value::Type::String, v2.type());
    EXPECT_TRUE(v2.as<std::string>() == "");

    Value v3(Value::Type::String, std::string("Hello"));
    EXPECT_EQ(Value::Type::String, v3.type());
    EXPECT_TRUE(v3.as<std::string>() == "Hello");

    Value v4(Value::Type::String, std::string("3.14"));
    EXPECT_EQ(Value::Type::String, v4.type());
    EXPECT_TRUE(v4.as<std::string>() == "3.14");
    EXPECT_DOUBLE_EQ(3.14, v4.as<double>());
}

TEST(Value, Table) {
    Value v;
    EXPECT_NE(Value::Type::Table, v.type());

    Value v2(Value::Type::Table);
    EXPECT_EQ(Value::Type::Table, v2.type());
    EXPECT_TRUE(v2.as<std::string>() == "");

    Value v3(Value::Type::Table, std::string("Hello"));
    EXPECT_EQ(Value::Type::Table, v3.type());
    EXPECT_TRUE(v3.as<std::string>() == "Hello");

    EXPECT_THROW(v3.as<double>(), Value::invalid_cast);
}
