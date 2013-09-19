#include "lua.hpp"

#include <memory>

namespace lutop {

class LuaTable;

class LuaValue {
    public:
        LuaValue(lua_State *L);
        ~LuaValue() throw();

        const char *asString(size_t *len = nullptr);
        bool asBool();
        std::shared_ptr<LuaTable> asTable();

    private:
        lua_State *L_;
};

class LuaTable {
    public:
        LuaTable(lua_State *L);
        ~LuaTable() throw();

        std::shared_ptr<LuaValue> get(const char *field);
        inline std::shared_ptr<LuaValue> operator[](const char *field) {
            return this->get(field);
        }

    private:
        lua_State *L_;
};

}
