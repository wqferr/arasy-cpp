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

        // template<typename T = LuaNil, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
        // std::optional<T> getDetail(int idx) const {
        //     return nil;
        // }

    public:
        lua_State *const state;

        Lua(): state(luaL_newstate()) {}
        ~Lua() { lua_close(state); }

        int size() const;
        void push(const LuaValue& value);

        template<typename T = LuaNil, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
        std::optional<T> pop() {
            lua_pop(state, 1);
            return nil;
        }

        template<typename T = LuaNil, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
        std::optional<T> get(int idx) const {
            if (checkIndexExists(idx)) {
                return LuaStackReader<T>::readAt(state, idx);
            } else {
                return std::nullopt;
            }
        }

        template<typename T = LuaNil, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
        bool has(int idx) const {
            if (checkIndexExists(idx)) {
                return LuaStackReader<T>::checkAt(state, idx);
            } else {
                return std::nullopt;
            }
        }

        template<typename T = LuaNil, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
        std::optional<T> getTop() const {
            return get<T>(-1);
        }

        operator lua_State*() const { return state; }
    };
}
