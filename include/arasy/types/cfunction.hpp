#pragma once

#include "arasy/types/base.hpp"

namespace arasy::core {
    class LuaCFunction : public internal::LuaBaseType {
    public:
        lua_CFunction cfunc;

        LuaCFunction(lua_CFunction cfunc_): cfunc(cfunc_) {}

        void pushOnto(lua_State* L) const override {
            lua_pushcfunction(L, cfunc);
        }
    };

    inline bool operator==(const LuaCFunction& a, const LuaCFunction& b) { return a.cfunc == b.cfunc; }

    template<>
    constexpr const bool is_potentially_callable_v<LuaCFunction> = true;

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
