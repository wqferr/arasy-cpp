#pragma once

#include <type_traits>
#include <optional>
#include <vector>
#include <utility>

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

    public:
        lua_State* const state;
        const bool external = false;
        arasy::registry::LuaRegistry registry;

        Lua();
        Lua(lua_State* L);
        ~Lua();

        void ensureStack(int i);
        int stackSize() const;
        std::optional<LuaValueVarIndex> type(int idx) const;

        void push(const LuaValue& value);
        void pushInt(lua_Integer i);
        void pushNum(lua_Number x);
        void pushStr(const std::string& str);

        void pushNewTable();
        LuaTable createNewTable();
        std::optional<LuaTable> makeTable(const std::vector<std::pair<LuaValue, LuaValue>>& entries);
        // TODO: newTable(std::unordered_map<LuaValue, LuaValue>)

        void pushCFunction(lua_CFunction cf);
        LuaCFunction createCFunction(lua_CFunction cf);

        void pushCClosure(lua_CFunction cf, int nUpvalues);
        LuaCFunction createCClosureStackUpvalues(lua_CFunction cf, int nUpvalues);

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

        // Transpose a LuaValue from a different Lua state.
        void receive(LuaValue copyOfAlien);

        GlobalVariableProxy operator[](const std::string& name);

        template<typename T = LuaValue, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
        std::optional<T> popStack() {
            if (checkIndexExists(-1)) {
                auto val = readStack<T>(-1);
                lua_pop(state, 1);
                return val;
            } else {
                return std::nullopt;
            }
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
        std::optional<T> getGlobal(const std::string& name) {
            retrieveGlobal(name);
            return popStack<T>();
        }

        void retrieveGlobal(const std::string& name);

        LuaTable getGlobalsTable();
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

        template<typename... Args, typename = std::enable_if_t<all_are_convertible_to_lua_value_v<Args...>>>
        thread::ResumeResult resume(LuaThread& thread, const Args&... args) {
            int nret;

            (thread.thread().push(LuaValue{args}), ...);
            int status = lua_resume(thread.thread(), state, sizeof...(args), &nret);
            switch (status) {
                case LUA_YIELD:
                    for (int i = nret; i >= 1; i--) {
                        receive(*thread.thread().readStack(-i));
                    }
                    return thread::Ok({ false, nret });
                case LUA_OK:
                    for (int i = nret; i >= 1; i--) {
                        receive(*thread.thread().readStack(-i));
                    }
                    return thread::Ok({ true, nret });
                default:
                    return thread::Error({ popStack<LuaString>()->fullStr() });
            }
        }

        std::optional<arasy::error::ScriptError> pcall(int narg=0, int nret=LUA_MULTRET, lua_KContext ctx=0);
        std::optional<arasy::error::ScriptError> loadString(const std::string& code);
        std::optional<arasy::error::ScriptError> executeString(const std::string& code);
        std::optional<arasy::error::ScriptError> loadFile(const std::string& fileName);
        std::optional<arasy::error::ScriptError> executeFile(const std::string& fileName);

        operator lua_State*() const { return state; }
    };
}
