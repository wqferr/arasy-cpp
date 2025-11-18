#pragma once
#include "arasy/types/callable.hpp"
#include "arasy/registry/reference.hpp"

namespace arasy::core {
    class LuaFunction : public internal::LuaCallable {
    public:
        LuaFunction(lua_State* L, int index): LuaCallable(L, index) {}

        void pushOnto(lua_State* L) const override { LuaReference::pushOnto(L); }
    };

    namespace internal {
        template<>
        struct LuaStackReader<LuaFunction> {
            static bool checkAt(lua_State* L, int idx) {
                return lua_isfunction(L, idx);
            }

            static std::optional<LuaFunction> readAt(lua_State* L, int idx) {
                if (checkAt(L, idx)) {
                    return LuaFunction(L, idx);
                } else {
                    return std::nullopt;
                }
            }
        };
    }
}
