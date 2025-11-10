#pragma once

#include "types/base.hpp"

namespace arasy::core {
    class LuaCFunction : public internal::LuaBaseType {
        lua_CFunction cfunc;

    public:
        LuaCFunction(lua_CFunction cfunc_): cfunc(cfunc_) {}

        void invokeNoPush() const;
        void invoke();

        void invoke(const internal::LuaBaseType& t);

        void pushOnto(lua_State* L) const override {
            lua_pushcfunction(L, cfunc);
        }
    };

    template<>
    struct LuaStackReader<LuaCFunction> {
        static bool checkAt(lua_State* L, int idx) {
            return lua_iscfunction(L, idx);
        }

        static std::optional<LuaCFunction> readAt(lua_State* L, int idx) {
            if (lua_iscfunction(L, idx)) {
                return lua_tocfunction(L, idx);
            } else {
                return std::nullopt;
            }
        }
    };
}
