#pragma once

#include "types/base.hpp"

namespace arasy::core {
    class LuaCFunction : public internal::LuaBaseType {
    public:
        const lua_CFunction cfunc;

        LuaCFunction(lua_CFunction cfunc_): cfunc(cfunc_) {}

        bool operator==(const LuaCFunction& other) const { return cfunc == other.cfunc; }

        void invokeNoPush() const;
        void invoke();

        void invoke(const internal::LuaBaseType& t);

        void pushOnto(lua_State* L) const override {
            lua_pushcfunction(L, cfunc);
        }
    };

    namespace internal {
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
}
