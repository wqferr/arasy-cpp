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
        bool checkIndexExists(int idx) const {
            if (idx < 0) {
                return size() >= -idx;
            } else {
                return size() >= idx;
            }
        }

    public:
        lua_State *const state;

        Lua(): state(luaL_newstate()) {}
        ~Lua() { lua_close(state); }

        int size() const;

        void push(const LuaValue& value);
        void pushInt(lua_Integer i) { push(LuaInteger{i}); }
        void pushNum(lua_Number x) { push(LuaNumber{x}); }
        void pushStr(const char *str) { push(LuaString{str}); }
        void pushNil() { push(nil); }

        template<typename T = LuaNil>
        requires(is_lua_wrapper_type<T>)
        std::optional<T> pop() {
            lua_pop(state, 1);
            return nil;
        }

        template<typename T = LuaNil>
        requires(is_lua_wrapper_type<T>)
        std::optional<T> get(int idx) const {
            if (checkIndexExists(idx)) {
                return internal::LuaStackReader<T>::readAt(state, idx);
            } else {
                return std::nullopt;
            }
        }

        template<typename T = LuaNil>
        requires(is_lua_wrapper_type<T>)
        bool has(int idx) const {
            if (checkIndexExists(idx)) {
                return internal::LuaStackReader<T>::checkAt(state, idx);
            } else {
                return false;
            }
        }

        template<typename T = LuaNil>
        requires(is_lua_wrapper_type<T>)
        bool hasTop() const {
            return has<T>(-1);
        }

        template<typename T = LuaNil>
        requires(is_lua_wrapper_type<T>)
        std::optional<T> getTop() const {
            return get<T>(-1);
        }

        operator lua_State*() const { return state; }
    };
}
