#pragma once

#include <variant>
#include <type_traits>
#include <memory>
#include <optional>
#include <ostream>

#include "arasy/lua.hpp"
#include "arasy/types/base.hpp"
#include "arasy/types/all.hpp"
#include "arasy/errors.hpp"
#include "arasy/pushfmt.hpp"
#include "arasy/registry.hpp"

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

            template<typename T, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
            GlobalVariableProxy& operator=(const T& value) {
                L.setGlobal<T>(globalName, value);
                return *this;
            }

            GlobalVariableProxy& operator=(const lua_Number& value);
            GlobalVariableProxy& operator=(const char* str);

            template<typename T, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
            void set(const T& value) {
                *this = value;
            }

            void set(const lua_Number& value) {
                *this = value;
            }

            void set(const char* value) {
                *this = value;
            }

            friend class Lua;
        };

        std::optional<GlobalVariableProxy> latestVariableAccessed;

    public:
        lua_State* const state;
        const bool external = false;
        arasy::registry::LuaRegistry registry;

        Lua(): state(luaL_newstate()), registry(state) {}
        Lua(lua_State* L): state(L), external(true), registry(state) {}
        ~Lua() {
            if (!external) {
                lua_close(state);
            }
        }

        int stackSize() const;
        std::optional<LuaValueVarIndex> type(int idx) const;

        void push(const LuaValue& value);
        void pushInt(lua_Integer i) { push(LuaInteger{i}); }
        void pushNum(lua_Number x) { push(LuaNumber{x}); }
        void pushStr(const std::string& str) { push(LuaString{str.c_str()}); }

        void pushNewTable() { lua_newtable(state); }
        LuaTable createNewTable() { pushNewTable(); return *popStack<LuaTable>(); }
        // TODO: newTable(std::unordered_map<LuaValue, LuaValue>)

        void pushCFunction(lua_CFunction cf) { lua_pushcfunction(state, cf); }
        LuaCFunction createCFunction(lua_CFunction cf) { pushCFunction(cf); return *popStack<LuaCFunction>(); }

        void pushCClosure(lua_CFunction cf, int nUpvalues) { lua_pushcclosure(state, cf, nUpvalues); }
        LuaCFunction createCClosureStackUpvalues(lua_CFunction cf, int nUpvalues) {
            pushCClosure(cf, nUpvalues);
            return *popStack<LuaCFunction>();
        }

        template<typename... Args, typename = std::enable_if_t<all_are_convertible_to_lua_value_v<Args...>>>
        LuaCFunction createCClosureInlineUpvalues(lua_CFunction cf, const Args&... args) {
            return LuaCFunction::withUpvalues(state, cf, args...);
        }

        template<typename... Args>
        std::optional<arasy::error::PushFmtErrorCode> pushFmt(const char *fmt, Args&&... args) {
            auto err = arasy::utils::checkPushFmt(std::string_view{fmt}, args...);
            if (err == std::nullopt) {
                lua_pushfstring(state, fmt, args...);
            }
            return err;
        }

        void pushNil() { push(nil); }

        // WARNING! NOT THREAD SAFE!!
        GlobalVariableProxy& operator[](const std::string& name);

        template<typename T = LuaValue, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
        std::optional<T> popStack() {
            auto val = readStack<T>(-1);
            lua_pop(state, 1);
            return val;
        }

        void multiPop(int n);

        template<typename T = LuaValue, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
        std::optional<T> readStack(int idx) const {
            if (checkIndexExists(idx)) {
                return internal::LuaStackReader<T>::readAt(state, idx);
            } else {
                return std::nullopt;
            }
        }

        template<typename T = LuaValue, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
        bool checkStack(int idx) const {
            if (checkIndexExists(idx)) {
                return internal::LuaStackReader<T>::checkAt(state, idx);
            } else {
                return false;
            }
        }

        template<typename T = LuaValue, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
        bool checkStackTop() const {
            return checkStack<T>(-1);
        }

        template<typename T = LuaValue, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
        std::optional<T> readStackTop() const {
            return readStack<T>(-1);
        }

        template<typename T = LuaValue, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
        std::optional<T> readGlobal(const std::string& name) {
            retrieveGlobal(name);
            return popStack<T>();
        }

        void retrieveGlobal(const std::string& name);

        LuaTable readGlobalsTable();
        void retrieveGlobalsTable();

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

        std::optional<arasy::error::ScriptError> pcall(int narg=0, int nret=LUA_MULTRET, lua_KContext ctx=0);
        std::optional<arasy::error::ScriptError> loadString(const std::string& code);
        std::optional<arasy::error::ScriptError> executeString(const std::string& code);
        std::optional<arasy::error::ScriptError> loadFile(const std::string& fileName);
        std::optional<arasy::error::ScriptError> executeFile(const std::string& fileName);

        operator lua_State*() const { return state; }
    };
}
