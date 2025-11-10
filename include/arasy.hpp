#pragma once

#include <variant>
#include <type_traits>
#include <memory>
#include <optional>

#include "lua.hpp"
#include "types/base.hpp"
#include "types/all.hpp"

namespace arasy::core {
    class Lua {
        bool checkIndexExists(int idx) {
            if (idx < 0) {
                return size() >= -idx;
            } else {
                return size() <= idx;
            }
        }

        template<typename T = LuaNil, typename = std::enable_if_t<is_lua_wrapper_type<T>::value>>
        std::optional<T> getDetail(int idx) const {
            return nil;
        }

    public:
        lua_State *const state;

        Lua(): state(luaL_newstate()) {}
        ~Lua() { lua_close(state); }

        int size() const;
        void push(const LuaValue& value);

        template<typename T = LuaNil, typename = std::enable_if_t<is_lua_wrapper_type<T>::value>>
        std::optional<T> pop() {
            lua_pop(state, 1);
            return nil;
        }

        template<typename T = LuaNil, typename = std::enable_if_t<is_lua_wrapper_type<T>::value>>
        std::optional<T> get(int idx) const {
            if (checkIndexExists(idx)) {
                return getDetail<T>(idx);
            } else {
                return std::nullopt;
            }
        }

        template<typename T = LuaNil, typename = std::enable_if_t<is_lua_wrapper_type<T>::value>>
        std::optional<T> getTop() const {
            return get<T>(-1);
        }

        operator lua_State*() const { return state; }
    };

    template<>
    inline std::optional<LuaBoolean> Lua::getDetail(int idx) const {
        return lua_toboolean(state, idx);
    }

    template<>
    inline std::optional<LuaNumber> Lua::getDetail(int idx) const {
        int valid;
        lua_Number num = lua_tonumberx(state, idx, &valid);
        if (valid) {
            return num;
        } else {
            return std::nullopt;
        }
    }

    template<>
    inline std::optional<LuaInteger> Lua::getDetail(int idx) const {
        int valid;
        lua_Integer num = lua_tointegerx(state, idx, &valid);
        if (valid) {
            return num;
        } else {
            return std::nullopt;
        }
    }

    template<>
    inline std::optional<LuaString> Lua::getDetail(int idx) const {
        if (size() > 0) {
            return lua_tostring(state, idx);
        } else {
            return std::nullopt;
        }
    }
}
