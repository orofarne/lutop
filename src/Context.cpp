#include "Context.hpp"

#include "lua_cmsgpack.h"

#include <stdexcept>
#include <sstream>

#define LUTOP_ASSERT_EQ(x, y, msg) { \
    if((x) != (y)) { \
        std::ostringstream sstr; \
        sstr << msg << ": " \
            << #x << " is " << (x) << " (should be " << (y) << ")"; \
        throw std::runtime_error(sstr.str()); \
    } \
}

namespace lutop {

int
Context::lIndexPass_(lua_State *L) {
    int n = lua_gettop(L);
    if (n != 2) {
        lua_pushfstring(L, "lIndexPass_: invalid number of arguments %d (should be 2)", n);
        lua_error(L);
    }

    int idx = -1; // ???
    if(!lua_isstring(L, idx)) {
        lua_pushstring(L, "lIndexPass_: invalid argument");
        lua_error(L);
    }
    const char *k = lua_tostring(L, idx);

    const char *name2 = lua_tostring(L, lua_upvalueindex(1));

    lua_getglobal(L, name2);
    lua_getfield(L, -1, k);

    return 1;
}

Context::Context()
    : L_(nullptr)
{
    L_ = lua_open();

    // stdlib
    luaL_openlibs(L_);

    // cmsgpack
    int rc = luaopen_cmsgpack(L_);
    LUTOP_ASSERT_EQ(rc, 1, "luaopen_cmsgpack");

    // luajit
    rc = luaJIT_setmode(L_, 0, LUAJIT_MODE_ENGINE | LUAJIT_MODE_ON);
    LUTOP_ASSERT_EQ(rc, 1, "luaJIT_setmode");
}

Context::~Context() throw() {
    if(L_ != nullptr) {
        lua_close(L_);
    }
}

void
Context::load(const char *code, size_t len) {
    int rc = luaL_loadbuffer(L_, code, len, "lutop");
    LUTOP_ASSERT_EQ(rc, 0, "luaL_loadbuffer");

    rc = lua_pcall(L_, 0, 0, 0);
    if(rc) {
        std::string msg = "lua_pcall error: ";
        msg += lua_tolstring(L_, -1, nullptr);
        throw std::runtime_error(msg);
    }
}

void
Context::loadModule(const char *name, const char *code, size_t len) {
    int rc = luaL_loadbuffer(L_, code, len, "lutop");
    LUTOP_ASSERT_EQ(rc, 0, "luaL_loadbuffer");

    rc = lua_pcall(L_, 0, 1, 0);
    if(rc) {
        std::string msg = "lua_pcall error: ";
        msg += lua_tolstring(L_, -1, nullptr);
        throw std::runtime_error(msg);
    }

    lua_setglobal(L_, name);
}

void
Context::setString(const char *name, const char *str) {
    lua_pushstring(L_, str);
    lua_setglobal(L_, name);
}

void
Context::setString(const char *name, const char *str, size_t len) {
    lua_pushlstring(L_, str, len);
    lua_setglobal(L_, name);
}

const char *
Context::getString(const char *name, size_t *len) {
    lua_getglobal(L_, name);
    return lua_tolstring(L_, -1, len);
}

void
Context::mixTatables(const char *name1, const char *name2) {
    // Find table
    lua_getglobal(L_, name1);

    // Prepare metatable
    lua_newtable(L_);
    lua_pushstring(L_, "__index");
    lua_pushstring(L_, name2);
    lua_pushcclosure(L_, &Context::lIndexPass_, 1);
    lua_rawset(L_, -3);

    // Set metatable
    lua_setmetatable(L_, -2);
}

}
