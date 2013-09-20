#include <gtest/gtest.h>

#include "Context.hpp"

TEST(Context, Empty) {
    using namespace lutop;

    Context c;
}

TEST(Context, Hello) {
    using namespace lutop;

    std::string code = "x = 'hello'\n";

    Context c;
    c.load(code.c_str(), code.length());

    std::string str = c["x"]->asString();
    EXPECT_TRUE( str == "hello" );
}


TEST(Context, GetSetString) {
    using namespace lutop;

    Context c;

    c.setString("a", "hello");
    c.setString("b", "world", 5);

    std::string code = "x = a .. ' ' .. b\n";
    c.load(code.c_str(), code.length());

    std::string str = c["x"]->asString();
    EXPECT_TRUE( str == "hello world" );

    size_t len = 0;
    c["x"]->asString(&len);
    EXPECT_EQ(str.length(), len);
}


TEST(Context, LoadModule) {
    using namespace lutop;

    Context c;

    std::string code = "return {x = 'xyz', y = 10}";
    c.loadModule("xyz", code.c_str(), code.length());

    std::string code2 = "str = xyz.x .. xyz.y";
    c.load(code2.c_str(), code2.length());

    std::string str = c["str"]->asString();
    EXPECT_TRUE( str == "xyz10" );
}

TEST(Context, Shadow) {
    using namespace lutop;

    Context c;

    {
        std::string code = "x = 'hello'\n";
        c.load(code.c_str(), code.length());

        std::string str = c["x"]->asString();
        EXPECT_TRUE( str == "hello" );
    }

    {
        std::string code = "x = 'hello2'\n";
        c.load(code.c_str(), code.length());

        std::string str = c["x"]->asString();
        EXPECT_TRUE( str == "hello2" );
    }
}

TEST(Context, MixTables) {
    using namespace lutop;

    Context c;

    std::string code = "return { "
        "myappend = function(self, str) self.foo = self.foo .. str end "
        "}";
    c.loadModule("mymodule", code.c_str(), code.length());

    std::string code2 = "x = {foo = 'bar'}";
    c.load(code2.c_str(), code2.length());

    c.mixTatables("x", "mymodule");

    std::string code3 = "x:myappend('bazz')\n"
                        "str = x.foo\n";
    c.load(code3.c_str(), code3.length());

    std::string str = c["str"]->asString();
    EXPECT_TRUE( str == "barbazz" );
}

TEST(Context, FFI) {
    using namespace lutop;

    std::string code =
        "local ffi = require('ffi')\n"
        "ffi.cdef[[\n"
        "   int getpid();\n"
        "]]\n"
        "x = 'my pid is ' .. ffi.C.getpid()\n"
        ;


    Context c;
    c.load(code.c_str(), code.length());

    std::string str = c["x"]->asString();
    std::ostringstream sstr;
    sstr << "my pid is " << getpid();
    EXPECT_TRUE( str == sstr.str() );
}

TEST(Context, BoolFromTable) {
    using namespace lutop;

    std::string code =
        "t = { a = true, b = false }\n"
        ;


    Context c;
    c.load(code.c_str(), code.length());

    EXPECT_TRUE( c["t"]->asTable()->get("a")->asBool() );
    EXPECT_FALSE( c["t"]->asTable()->get("b")->asBool() );
    EXPECT_FALSE( c["t"]->asTable()->get("c")->asBool() );
}

TEST(Context, IsNil) {
    using namespace lutop;

    std::string code =
        "t = { a = true, b = false }\n"
        ;


    Context c;
    c.load(code.c_str(), code.length());

    EXPECT_TRUE( c["foo"]->isNil() );
    EXPECT_FALSE( c["t"]->isNil() );

    EXPECT_FALSE( c["t"]->asTable()->get("a")->isNil() );
    EXPECT_FALSE( c["t"]->asTable()->get("b")->isNil() );
    EXPECT_TRUE( c["t"]->asTable()->get("c")->isNil() );
}

TEST(Context, Integer) {
    using namespace lutop;

    std::string code =
        "x = 10\n"
        "y = 3.14\n"
        "z = 'hello'"
        ;


    Context c;
    c.load(code.c_str(), code.length());

    bool isnum = false;

    EXPECT_EQ(10, c["x"]->asInteger(&isnum));
    EXPECT_TRUE(isnum);

    EXPECT_EQ(3, c["y"]->asInteger(&isnum));
    EXPECT_TRUE(isnum);

    EXPECT_EQ(0, c["z"]->asInteger(&isnum));
    EXPECT_FALSE(isnum);

    EXPECT_EQ(0, c["foo"]->asInteger(&isnum));
    EXPECT_FALSE(isnum);
}
