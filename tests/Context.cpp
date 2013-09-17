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

    std::string str = c.getString("x");
    EXPECT_TRUE( str == "hello" );
}


TEST(Context, GetSetString) {
    using namespace lutop;

    Context c;

    c.setString("a", "hello");
    c.setString("b", "world", 5);

    std::string code = "x = a .. ' ' .. b\n";
    c.load(code.c_str(), code.length());

    std::string str = c.getString("x");
    EXPECT_TRUE( str == "hello world" );

    size_t len = 0;
    c.getString("x", &len);
    EXPECT_EQ(str.length(), len);
}


TEST(Context, LoadModule) {
    using namespace lutop;

    Context c;

    std::string code = "return {x = 'xyz', y = 10}";
    c.loadModule("xyz", code.c_str(), code.length());

    std::string code2 = "str = xyz.x .. xyz.y";
    c.load(code2.c_str(), code2.length());

    std::string str = c.getString("str");
    EXPECT_TRUE( str == "xyz10" );
}

TEST(Context, Shadow) {
    using namespace lutop;

    Context c;

    {
        std::string code = "x = 'hello'\n";
        c.load(code.c_str(), code.length());

        std::string str = c.getString("x");
        EXPECT_TRUE( str == "hello" );
    }

    {
        std::string code = "x = 'hello2'\n";
        c.load(code.c_str(), code.length());

        std::string str = c.getString("x");
        EXPECT_TRUE( str == "hello2" );
    }
}

TEST(Context, SetMetatable) {
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

    std::string str = c.getString("str");
    EXPECT_TRUE( str == "barbazz" );
}

