#pragma once

#include "arasy/types/callable.hpp"

#include <stdexcept>

namespace arasy::core {
    class LuaCFunction : public internal::LuaCallable {
        LuaCFunction(lua_State* L, lua_CFunction cfunc, std::in_place_t, int nUpvalues): LuaCallable(L) {
            lua_pushcclosure(registry.luaInstance, cfunc, nUpvalues);
            doRegister();
        }

    public:
        lua_CFunction cfunc;

        LuaCFunction(lua_State* L, int index): LuaCallable(L, index) {}

        template<typename... Args, typename = std::enable_if_t<all_are_convertible_to_lua_value_v<Args...>>>
        static LuaCFunction withUpvalues(lua_State* L, lua_CFunction cfunc, const Args&... args) {
            (LuaValue(args).pushOnto(L), ...);
            return LuaCFunction(L, cfunc, std::in_place, sizeof...(args));
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
                    return LuaCFunction(L, idx);
                } else {
                    return std::nullopt;
                }
            }
        };
    }
}
