#include "lua.hpp"

#include "types/base.hpp"

namespace arasy::core {
    class LuaBoolean : public LuaBaseType {
    public:
        const bool value;
        constexpr LuaBoolean(bool value_): value(value_) {}
        constexpr operator bool() const { return value; }
        void pushOnto(lua_State* L) const override { lua_pushboolean(L, value); };
    };
}
