#include "luatools.hpp"

namespace lutop {

LuaValue::LuaValue(lua_State *L)
    : L_(L)
{
}

LuaValue::~LuaValue() throw() {
    lua_pop(L_, 1);
}

bool
LuaValue::isNil() {
    return lua_isnil(L_, -1);
}

const char *
LuaValue::asString(size_t *len) {
    return lua_tolstring(L_, -1, len);
}

lua_Integer
LuaValue::asInteger(bool *is_num) {
    if(is_num)
        *is_num = (bool)lua_isnumber(L_, -1);

    lua_Integer res = lua_tointeger(L_, -1);
    return res;
}

bool
LuaValue::asBool() {
    return lua_toboolean(L_, -1);
}

std::shared_ptr<LuaTable>
LuaValue::asTable() {
    return std::make_shared<LuaTable>(L_);
}

LuaTable::LuaTable(lua_State *L)
    : L_(L)
{
}

LuaTable::~LuaTable() throw() {
}

std::shared_ptr<LuaValue>
LuaTable::get(const char *field) {
    lua_getfield(L_, -1, field);
    return std::make_shared<LuaValue>(L_);
}

}
