#include <gtest/gtest.h>

#include "Encode.hpp"

#include "lua.hpp"
#include "lua_cmsgpack.h"

#include <iostream>

#define LUA_ASSERT_PCALL(L, rc) \
    EXPECT_EQ(0, rc); \
    if(rc) { \
        FAIL() << "Error: " << lua_tolstring(L, -1, nullptr);\
    }

using namespace lutop;

TEST(Encode, enc1) {
    Request r;
    r.func = "myfunc";
    r.state = "\x93\xa5Hello\xabMessagePack\xa5Hello";
    r.metric = "test_metric";
    r.value = Value(Value::Type::Numeric, 3.14);
    r.timestamp = 1380132909;

    std::string buf = encodeRequest(r);

#if 0
    for(auto ch : buf) { std::cout << std::hex << int(ch) << '\t' << ch << std::endl; }
#endif

    // Check by cmsgpack
    int rc;

    lua_State *L = lua_open();
    luaL_openlibs(L);

    rc = luaopen_cmsgpack(L);
    ASSERT_EQ(1, rc);

    lua_pushlstring(L, buf.data(), buf.size());
    lua_setglobal(L, "buf");

    std::string code = "local t = cmsgpack.unpack(buf)\n"
                       "res = true\n"
                       "if t['func'] ~= 'myfunc' then res = false end\n"
                       "if t['metric'] ~= 'test_metric' then res = false end\n"
                       "if t['timestamp'] ~= 1380132909 then res = false end\n"
                       "if t['value'] ~= 3.14 then res = false end\n"
                       "if t['state'][1] ~= 'Hello' then res = false end\n"
                       "if t['state'][2] ~= 'MessagePack' then res = false end\n"
                       "if t['state'][3] ~= 'Hello' then res = false end\n"
                       ;
    rc = luaL_loadbuffer(L, code.c_str(), code.length(), "enc1");
    ASSERT_EQ(0, rc);

    rc = lua_pcall(L, 0, 0, 0);
    LUA_ASSERT_PCALL(L, rc);

    lua_getglobal(L, "res");
    bool res = lua_toboolean(L, -1);
    lua_pop(L, 1);

    EXPECT_TRUE(res);
}

TEST(Encode, enc2) {
    Request r;
    r.func = "myfunc";
    r.state = "";
    r.metric = "test_metric";
    r.value = Value(Value::Type::Table, std::string("\x93\xa5Hello\xabMessagePack\xa5Hello"));
    r.timestamp = 1380132909;

    std::string buf = encodeRequest(r);

#if 0
    for(auto ch : buf) { std::cout << std::hex << int(ch) << '\t' << ch << std::endl; }
#endif

    // Check by cmsgpack
    int rc;

    lua_State *L = lua_open();
    luaL_openlibs(L);

    rc = luaopen_cmsgpack(L);
    ASSERT_EQ(1, rc);

    lua_pushlstring(L, buf.data(), buf.size());
    lua_setglobal(L, "buf");

    std::string code = "local t = cmsgpack.unpack(buf)\n"
                       "res = true\n"
                       "if t['func'] ~= 'myfunc' then res = false end\n"
                       "if t['metric'] ~= 'test_metric' then res = false end\n"
                       "if t['timestamp'] ~= 1380132909 then res = false end\n"
                       "if t['state'] then res = false end\n"
                       "if t['value'][1] ~= 'Hello' then res = false end\n"
                       "if t['value'][2] ~= 'MessagePack' then res = false end\n"
                       "if t['value'][3] ~= 'Hello' then res = false end\n"
                       ;
    rc = luaL_loadbuffer(L, code.c_str(), code.length(), "enc2");
    ASSERT_EQ(0, rc);

    rc = lua_pcall(L, 0, 0, 0);
    LUA_ASSERT_PCALL(L, rc);

    lua_getglobal(L, "res");
    bool res = lua_toboolean(L, -1);
    lua_pop(L, 1);

    EXPECT_TRUE(res);
}

TEST(Encode, dec1) {
    std::string buf = "\x84\xa9timestamp\xce\x52\x43\x28\x2d\xa5value\xcb\x40\x09\x1e\xb8\x51\xeb\x85\x1f\xa5state\x93\xa5Hello\xabMessagePack\xa5Hello\xa5\u0065rror\xa8my error";

    // Test test >_<
    // Check by cmsgpack
    {
        int rc;

        lua_State *L = lua_open();
        luaL_openlibs(L);

        rc = luaopen_cmsgpack(L);
        ASSERT_EQ(1, rc);

        lua_pushlstring(L, buf.data(), buf.size());
        lua_setglobal(L, "buf");

        std::string code = "local t = cmsgpack.unpack(buf)\n"
                           "res = true\n"
                           "if t['timestamp'] ~= 1380132909 then res = false end\n"
                           "if t['value'] ~= 3.14 then res = false end\n"
                           "if t['state'][1] ~= 'Hello' then res = false end\n"
                           "if t['state'][2] ~= 'MessagePack' then res = false end\n"
                           "if t['state'][3] ~= 'Hello' then res = false end\n"
                           "if t['error'] ~= 'my error' then res = false end\n"
                           ;
        rc = luaL_loadbuffer(L, code.c_str(), code.length(), "enc1");
        ASSERT_EQ(0, rc);

        rc = lua_pcall(L, 0, 0, 0);
        LUA_ASSERT_PCALL(L, rc);

        lua_getglobal(L, "res");
        bool res = lua_toboolean(L, -1);
        lua_pop(L, 1);

        ASSERT_TRUE(res);
    }

    // Check c++ decoder
    std::shared_ptr<Response> r = decodeResponse(buf);
    ASSERT_TRUE((bool)r);

    auto str_assert_cmp = [](const std::string &a, const std::string &b) {
        ASSERT_EQ(b.size(), a.size());
        size_t s = b.size();
        for(size_t i = 0; i < s; ++i) {
            EXPECT_EQ(b[i], a[i]);
        }
    };

    str_assert_cmp(r->state, "\x93\xa5Hello\xabMessagePack\xa5Hello");

    ASSERT_EQ(Value::Type::Numeric, r->value.type());
    EXPECT_DOUBLE_EQ(3.14, r->value.as<double>());
    EXPECT_EQ(1380132909, r->timestamp);

    str_assert_cmp(r->error, "my error");
}
