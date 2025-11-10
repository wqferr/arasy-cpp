#pragma once

#include "types/forwarddeclare.hpp"

#include "types/base.hpp"

namespace arasy::core {
    class LuaCFunction : public LuaBaseType {
        lua_CFunction cfunc;

    public:
        LuaCFunction(lua_CFunction cfunc_): cfunc(cfunc_) {}

        void invokeNoPush() const;

        template<typename T>
        void invoke(const T& t);

        void pushOnto(lua_State* L) const override {
            lua_pushcfunction(L, cfunc);
        }
    };
}
