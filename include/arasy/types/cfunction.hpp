#pragma once

#include "arasy/types/callable.hpp"

#include <stdexcept>

namespace arasy::core {
    class LuaCFunction : public internal::LuaCallable {
    public:
        lua_CFunction cfunc;

        LuaCFunction(lua_State* L, lua_CFunction cfunc_): LuaCallable(L), cfunc(cfunc_) {}

        void pushOnto(lua_State* L) const override {
            if (L != callableContext) {
                throw std::runtime_error("Mixed CFunctions from different Lua instances");
            }
            lua_pushcfunction(L, cfunc);
        }

        friend bool operator==(const LuaCFunction& a, const LuaCFunction& b);
    };

    inline bool operator==(const LuaCFunction& a, const LuaCFunction& b) {
        return a.callableContext == b.callableContext && a.cfunc == b.cfunc;
    }

    namespace internal {
        template<>
        struct LuaStackReader<LuaCFunction> {
            static bool checkAt(lua_State* L, int idx) {
                return lua_iscfunction(L, idx);
            }

            static std::optional<LuaCFunction> readAt(lua_State* L, int idx) {
                if (lua_iscfunction(L, idx)) {
                    return LuaCFunction(L, lua_tocfunction(L, idx));
                } else {
                    return std::nullopt;
                }
            }
        };
    }
}
