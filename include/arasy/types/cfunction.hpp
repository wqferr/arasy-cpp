#pragma once

#include "arasy/types/callable.hpp"

#include <stdexcept>

namespace arasy::core {
    class LuaCFunction : public internal::LuaCallable {
    public:
        lua_CFunction cfunc;

        LuaCFunction(lua_State* L, int index): LuaCallable(L, index) {}
    };

    namespace internal {
        template<>
        struct LuaStackReader<LuaCFunction> {
            static bool checkAt(lua_State* L, int idx) {
                return lua_iscfunction(L, idx);
            }

            static std::optional<LuaCFunction> readAt(lua_State* L, int idx) {
                if (lua_iscfunction(L, idx)) {
                    return LuaCFunction(L, idx);
                } else {
                    return std::nullopt;
                }
            }
        };
    }
}
