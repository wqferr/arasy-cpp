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
            return !lua_isnone(state, idx);
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
        lua_State* const state;
        const bool external = false;

        Lua(): state(luaL_newstate()) {}
        Lua(lua_State* L): state(L), external(true) {}
        ~Lua() { if (external) { lua_close(state); } }

        int size() const;

        void push(const LuaValue& value);
        void pushInt(lua_Integer i) { push(LuaInteger{i}); }
        void pushNum(lua_Number x) { push(LuaNumber{x}); }
        void pushStr(const std::string& str) { push(LuaString{str.c_str()}); }
        void pushFmt(const char *fmt, ...);

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

        LuaValue getGlobal(const std::string& name);


        template<typename T = LuaValue, typename = std::enable_if_t<is_nonvariant_lua_wrapper_type_v<T>>>
        void setGlobal(const std::string& name, const T& value) {
            push(value);
            lua_setglobal(state, name.c_str());
        }

        void setGlobalInt(const std::string& name, lua_Integer value) {
            setGlobal<LuaInteger>(name, value);
        }

        void setGlobalNum(const std::string& name, lua_Number value) {
            setGlobal<LuaNumber>(name, value);
        }

        void setGlobalStr(const std::string& name, const std::string& value) {
            setGlobal<LuaString>(name, value.c_str());
        }

        void eraseGlobal(const std::string& name) {
            setGlobal<LuaNil>(name, nil);
        }

        operator lua_State*() const { return state; }
    };
}
