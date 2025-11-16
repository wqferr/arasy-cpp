#pragma once
#include "arasy/types/base.hpp"
#include "arasy/registry/reference.hpp"
#include "lua.hpp"
#include <string>

namespace arasy::core {
    class LuaValue;
}

namespace arasy::core::internal {
    class LuaCallable : public arasy::registry::LuaReference {
    protected:
        LuaCallable(lua_State* L): LuaReference(L) {}

    private:
        enum CallMode {
            RAWCALL,
            PCALL,
        };

        template<int nret>
        std::optional<std::string> invokeHelper(CallMode mode, int nargs) {
            switch (mode) {
                case RAWCALL:
                    lua_call(registry.luaInstance, nargs, nret);

                case PCALL:
                    if (lua_pcall(registry.luaInstance, nargs, nret, 0) == LUA_OK) {
                        return std::nullopt;
                    } else {
                        // TODO check for LUA_ERRMEM
                        std::string errMsg = lua_tostring(registry.luaInstance, -1);
                        lua_pop(registry.luaInstance, 1);
                        return errMsg;
                    }
                    return std::nullopt;
            }
            return "internal arasy error";
        }

        template<int nret, typename Arg1, typename... Args>
        std::optional<std::string> invokeHelper(CallMode mode, int nargs, const Arg1& arg1, const Args&... args) {
            if constexpr (std::is_integral_v<Arg1>) {
                lua_pushinteger(registry.luaInstance, arg1);
            } else if constexpr (std::is_floating_point_v<Arg1>) {
                lua_pushnumber(registry.luaInstance, arg1);
            } else if constexpr (std::is_same_v<Arg1, const char*>) {
                lua_pushstring(registry.luaInstance, arg1);
            } else if constexpr (std::is_same_v<LuaValue, Arg1>) {
                arg1.pushOnto(registry.luaInstance);
            } else if constexpr (std::is_base_of_v<LuaBaseType, Arg1>) {
                LuaValue value {arg1};
                value.pushOnto(registry.luaInstance);
            }
            return invokeHelper<nret, Args...>(mode, nargs, args...);
        }

    public:
        LuaCallable(lua_State* L, int idx): LuaReference(L, idx) {}

        template<int nret=LUA_MULTRET, typename... Args, typename = std::enable_if_t<all_are_convertible_to_lua_value_v<Args...>>>
        std::optional<std::string> pcall(const Args&... args) {
            pushOnto(registry.luaInstance);
            return invokeHelper<nret, Args...>(PCALL, sizeof...(args), args...);
        }

        template<int  nret=LUA_MULTRET, typename... Args, typename = std::enable_if_t<all_are_convertible_to_lua_value_v<Args...>>>
        void call(const Args&... args) {
            pushOnto(registry.luaInstance);
            invokeHelper<nret, Args...>(RAWCALL, sizeof...(args), args...);
        }
    };
}
