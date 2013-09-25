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
