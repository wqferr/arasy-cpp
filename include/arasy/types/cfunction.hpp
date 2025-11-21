#pragma once

#include "arasy/types/callable.hpp"

namespace arasy::core {
    class LuaCFunction : public internal::LuaCallable {
        LuaCFunction(lua_State* L, lua_CFunction cfunc, std::in_place_t, int nUpvalues): LuaCallable(L) {
            lua_pushcclosure(registry.luaInstance, cfunc, nUpvalues);
            doRegister();
        }

    public:
        lua_CFunction cfunc;

        LuaCFunction(lua_State* L, int index): LuaCallable(L, index) {}

        template<typename... Args, typename = std::enable_if_t<(std::is_base_of_v<internal::LuaBaseType, Args> && ...)>>
        static LuaCFunction withUpvalues(lua_State* L, lua_CFunction cfunc, const Args&... args) {
            (args.pushOnto(L), ...);
            return LuaCFunction(L, cfunc, std::in_place, sizeof...(args));
        }

        lua_CFunction toFunctionPointer() const {
            return cfunc;
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
