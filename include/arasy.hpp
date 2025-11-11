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

        class GlobalVariableProxy {
        public:
            const std::string& name() { return globalName; }
            LuaValue value() const;
            operator LuaValue() const;

        private:
            Lua& L;
            const std::string globalName;

        public:
            GlobalVariableProxy(Lua& L_, const std::string& var): L(L_), globalName(var) {}

            template<typename T = LuaValue, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
            GlobalVariableProxy& operator=(const LuaValue& value) {
                L.push(value);
                lua_setglobal(L, globalName.c_str());
                return *this;
            }

            GlobalVariableProxy& operator=(const lua_Number& value);
            GlobalVariableProxy& operator=(const char *str);

            friend class Lua;
        };

        std::optional<GlobalVariableProxy> latestVariableAccessed;

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

        // WARNING! NOT THREAD SAFE!!
        GlobalVariableProxy& operator[](const std::string& name);

        template<typename T = LuaValue, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
        std::optional<T> pop() {
            auto val = get<T>(-1);
            lua_pop(state, 1);
            return val;
        }

        template<typename T = LuaValue, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
        std::optional<T> get(int idx) const {
            if (checkIndexExists(idx)) {
                return internal::LuaStackReader<T>::readAt(state, idx);
            } else {
                return std::nullopt;
            }
        }

        template<typename T = LuaValue, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
        bool has(int idx) const {
            if (checkIndexExists(idx)) {
                return internal::LuaStackReader<T>::checkAt(state, idx);
            } else {
                return false;
            }
        }

        template<typename T = LuaValue, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
        bool hasTop() const {
            return has<T>(-1);
        }

        template<typename T = LuaValue, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
        std::optional<T> getTop() const {
            return get<T>(-1);
        }

        operator lua_State*() const { return state; }
    };
}
