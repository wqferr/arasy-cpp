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
            if (idx < 0) {
                return -idx <= stackSize();
            } else {
                return idx <= stackSize();
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
        Lua(Lua&& other);
        Lua(const Lua& other);
        Lua& operator=(Lua&& other) = delete;
        Lua& operator=(const Lua& other) = delete;
        ~Lua();

        void ensureStack(int i);
        int stackSize() const;
        std::optional<LuaValueType> type(int idx) const;

        void push(const LuaValue& value);
        void pushInt(lua_Integer i);
        void pushNum(lua_Number x);
        void pushStr(const std::string& str);

        void pushNewTable();
        LuaTable createNewTable();
        std::optional<LuaTable> makeTable(const std::vector<std::pair<LuaValue, LuaValue>>& entries);

        void pushCFunction(lua_CFunction cf);
        LuaCFunction createCFunction(lua_CFunction cf);

        void pushCClosure(lua_CFunction cf, int nUpvalues);
        LuaCFunction createCClosureStackUpvalues(lua_CFunction cf, int nUpvalues);

        template<typename... Args, typename = std::enable_if_t<all_are_convertible_to_lua_value_v<Args...>>>
        LuaCFunction createCClosureInlineUpvalues(lua_CFunction cf, const Args&... args) {
            return LuaCFunction::withUpvalues(state, cf, args...);
        }

        template<typename... Args>
        error::MPushFmtError pushFmt(const char *fmt, Args&&... args) {
            auto err = utils::internal::checkPushFmt(std::string_view{fmt}, args...);
            if (!err.has_value()) {
                lua_pushfstring(state, fmt, args...);
            }
            return err;
        }

        void pushNil() { push(nil); }

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

        std::vector<LuaValue> multiPop(int n);

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

        template<typename T = LuaValue, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
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
        thread::ResumeResult resumeOther(bool moveRetOver, LuaThread& thread, const Args&... args) {
            return thread.resume(moveRetOver, *this, args...);
        }

        template<typename... Args, typename = std::enable_if_t<all_are_convertible_to_lua_value_v<Args...>>>
        thread::ResumeResult startOther(bool moveRetOver, LuaThread& thread, LuaFunction& f, const Args&... args) {
            return thread.start(moveRetOver, *this, f, args...);
        }

        template<typename... Args, typename = std::enable_if_t<all_are_convertible_to_lua_value_v<Args...>>>
        thread::ResumeResult startOther(bool moveRetOver, LuaThread& thread, lua_CFunction f, const Args&... args) {
            return thread.start(moveRetOver, *this, f, args...);
        }

        int yieldk(int nret, lua_KContext ctx, lua_KFunction k);
        int yield(int nret);

        LuaThread createNewThread() {
            lua_newthread(state);
            return *popStack<LuaThread>();
        }

        // WARNING: Cannot be used if function yields
        void call(int narg=0, int nret=LUA_MULTRET);

        // WARNING: Cannot be used if function yields
        error::MScriptError pcall(int narg=0, int nret=LUA_MULTRET);

        void callk(int narg, int nret, lua_KContext ctx, lua_KFunction cont);
        error::MScriptError pcallk(int narg, int nret, lua_KContext ctx, lua_KFunction cont);

        error::MScriptError wrapScriptError(int status);
        error::MScriptError loadString(const std::string& code);
        error::MScriptError executeString(const std::string& code);
        error::MScriptError loadFile(const std::string& fileName);
        error::MScriptError executeFile(const std::string& fileName);

        void concat(int n);
        void arith(int op);

        operator lua_State*() const { return state; }
    };
}

namespace arasy {
    /// \typedef arasy::core::LuaNil
    using LuaNil = core::LuaNil;

    /// \typedef arasy::core::LuaBoolean
    using LuaBoolean = core::LuaBoolean;

    /// \typedef arasy::core::LuaInteger
    using LuaInteger = core::LuaInteger;

    /// \typedef arasy::core::LuaNumber
    using LuaNumber = core::LuaNumber;

    /// \typedef arasy::core::LuaString
    using LuaString = core::LuaString;

    /// \typedef arasy::core::LuaTable
    using LuaTable = core::LuaTable;

    /// \typedef arasy::core::LuaFunction
    using LuaFunction = core::LuaFunction;

    /// \typedef arasy::core::LuaCFunction
    using LuaCFunction = core::LuaCFunction;

    /// \typedef arasy::core::LuaThread
    using LuaThread = core::LuaThread;

    /// \typedef arasy::core::LuaLightUserData
    using LuaLightUserData = core::LuaLightUserData;

    /// \typedef arasy::core::LuaFullUserData
    using LuaFullUserData = core::LuaFullUserData;

    /// \typedef arasy::core::LuaValue
    using LuaValue = core::LuaValue;

    /// \typedef arasy::core::nil
    constexpr LuaNil nil = core::nil;

    /// \typedef arasy::core::True
    constexpr LuaBoolean True = core::True;

    /// \typedef arasy::core::False
    constexpr LuaBoolean False = core::False;

    const inline LuaValue True_lv = core::True_lv;
    const inline LuaValue nil_lv = LuaValue{nil};
    const inline LuaValue False_lv = core::False_lv;

    /// \typedef arasy::core::thread::ResumeResult
    using ResumeResult = core::thread::ResumeResult;

    /// \typedef arasy::core::LuaValueType
    using LuaValueType = core::LuaValueType;

    /// \typedef arasy::core::Lua
    using Lua = core::Lua;
}
