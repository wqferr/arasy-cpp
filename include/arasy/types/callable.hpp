#pragma once
#include "arasy/types/base.hpp"
#include "lua.hpp"
#include <string>

namespace arasy::core::internal {
    class LuaCallable : public LuaBaseType {
    protected:
        lua_State* const callableContext;

    private:
        enum CallMode {
            RAWCALL,
            PCALL,
        };

        template<int nret>
        std::optional<std::string> invokeHelper(CallMode mode, int nargs) {
            switch (mode) {
                case RAWCALL:
                    lua_call(callableContext, nargs, nret);

                case PCALL:
                    if (lua_pcall(callableContext, nargs, nret, 0) == LUA_OK) {
                        return std::nullopt;
                    } else {
                        // TODO check for LUA_ERRMEM
                        std::string errMsg = lua_tostring(callableContext, -1);
                        lua_pop(callableContext, 1);
                        return errMsg;
                    }
                    return std::nullopt;
            }
            return "internal arasy error";
        }

        template<int nret, typename Arg1, typename... Args>
        std::optional<std::string> invokeHelper(CallMode mode, int nargs, const Arg1& arg1, const Args&... args) {
            if constexpr (std::is_integral_v<Arg1>) {
                lua_pushinteger(callableContext, arg1);
            } else if constexpr (std::is_floating_point_v<Arg1>) {
                lua_pushnumber(callableContext, arg1);
            } else if constexpr (std::is_same_v<Arg1, const char*>) {
                lua_pushstring(callableContext, arg1);
            } else {
                static_assert(is_lua_wrapper_type_v<Arg1>);
                LuaValue value {arg1};
                value.pushOnto(callableContext);
            }
            return invokeHelper<nret, Args...>(mode, nargs, args...);
        }

    public:
        LuaCallable(lua_State* L_): callableContext(L_) {}

        template<int nret=LUA_MULTRET, typename... Args>
        std::optional<std::string> pcall(const Args&... args) {
            pushOnto(callableContext);
            return invokeHelper<nret, Args...>(PCALL, sizeof...(args), args...);
        }

        template<int  nret=LUA_MULTRET, typename... Args>
        void call(const Args&... args) {
            pushOnto(callableContext);
            invokeHelper<nret, Args...>(RAWCALL, sizeof...(args), args...);
        }
    };
}
