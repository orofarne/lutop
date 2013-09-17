#include <gtest/gtest.h>

#include "lua.hpp"
#include "lua_cmsgpack.h"

#define LUA_ASSERT_PCALL(L, rc) \
    EXPECT_EQ(0, rc); \
    if(rc) { \
        FAIL() << "Error: " << lua_tolstring(L, -1, NULL);\
    }

TEST(cmsgpack, checkLua) {
    int rc;

    lua_State *L = lua_open();
    luaL_openlibs(L);

    std::string buf = "x = 10";
    rc = luaL_loadbuffer(L, buf.c_str(), buf.length(), "test");
    ASSERT_EQ(0, rc);

    rc = luaJIT_setmode(L, 0, LUAJIT_MODE_ALLSUBFUNC | LUAJIT_MODE_ON);
    EXPECT_EQ(1, rc);

    rc = lua_pcall(L, 0, 0, 0);
    LUA_ASSERT_PCALL(L, rc);

    lua_getglobal(L, "x");
    auto num = lua_tonumber(L, -1);
    lua_pop(L, 1);

    EXPECT_DOUBLE_EQ(10, num);

    lua_close(L);
}

TEST(cmsgpack, packUnpack) {
    int rc;

    lua_State *L = lua_open();
    luaL_openlibs(L);

    rc = luaopen_cmsgpack(L);
    ASSERT_EQ(1, rc);

    std::string buf = "local t = {x = 8, y = 2}\n"
                      "local msgpack = cmsgpack.pack(t)\n"
                      "local t2 = cmsgpack.unpack(msgpack)\n"
                      "x = t2['x'] + t2['y']\n";
    rc = luaL_loadbuffer(L, buf.c_str(), buf.length(), "test");
    ASSERT_EQ(0, rc);

    rc = lua_pcall(L, 0, 0, 0);
    LUA_ASSERT_PCALL(L, rc);

    lua_getglobal(L, "x");
    auto num = lua_tonumber(L, -1);
    lua_pop(L, 1);

    EXPECT_DOUBLE_EQ(10, num);

    lua_close(L);
}

TEST(cmsgpack, example) {
    int rc;

    lua_State *L = lua_open();
    luaL_openlibs(L);

    rc = luaopen_cmsgpack(L);
    ASSERT_EQ(1, rc);

    std::string bin = {'\xa5', 'H', 'e', 'l', 'l', 'o'};

    lua_pushlstring(L, bin.data(), bin.size());
    lua_setglobal(L, "bindata");

    std::string buf = "str = cmsgpack.unpack(bindata)\n";

    rc = luaL_loadbuffer(L, buf.c_str(), buf.length(), "test");
    ASSERT_EQ(0, rc);

    rc = lua_pcall(L, 0, 0, 0);
    LUA_ASSERT_PCALL(L, rc);

    lua_getglobal(L, "str");
    std::string str = lua_tolstring(L, -1, NULL);
    if(str != "Hello") {
        FAIL() << "str = " << str << " (not \"Hello\")";
    }
    lua_pop(L, 1);

    lua_close(L);
}

