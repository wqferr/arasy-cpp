#pragma once

#include <variant>
#include <type_traits>
#include <memory>
#include <optional>

#include "lua.hpp"
#include "types/base.hpp"
#include "types/all.hpp"

namespace arasy::error {
    enum class PushFmtError {
        NONE,
        TOO_MANY_ARGS,
        TOO_FEW_ARGS,
        INVALID_PLACEHOLDER,
        INCOMPATIBLE_ARG,
        UNSPECIFIED
    };
}

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

        template<typename... Args>
        constexpr arasy::error::PushFmtError checkPushFmt(const std::string_view fmt) {
            if (fmt.find('%') != std::string_view::npos) {
                return arasy::error::PushFmtError::TOO_FEW_ARGS;
            } else {
                return arasy::error::PushFmtError::NONE;
            }
        }

        template<typename T1, typename... Args>
        constexpr arasy::error::PushFmtError checkPushFmt(const std::string_view fmt, T1 firstArg, Args... args) {
            auto idx = fmt.find('%');
            if (idx == std::string_view::npos) {
                return arasy::error::PushFmtError::TOO_MANY_ARGS;
            }

            bool parsing = false;
            do {
                idx++;
                if (idx >= fmt.size()) {
                    return arasy::error::PushFmtError::INVALID_PLACEHOLDER;
                }

                switch (fmt.at(idx)) {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                    case '-':
                    case '+':
                    case '.':
                        parsing = true;
                        break;

                    case '%':
                        break;

                    case 'd':
                    case 'c':
                    case 'o':
                    case 'x':
                    case 'X':
                        if constexpr (!std::is_convertible_v<T1, LuaInteger>) {
                            return arasy::error::PushFmtError::INCOMPATIBLE_ARG;
                        }
                        break;

                    case 'f':
                    case 'g':
                    case 'G':
                    case 'e':
                    case 'E':
                        if constexpr (!std::is_convertible_v<T1, LuaNumber>) {
                            return arasy::error::PushFmtError::INCOMPATIBLE_ARG;
                        }
                        break;

                    case 's':
                    case 'q':
                        if constexpr (!std::is_convertible_v<T1, LuaString>) {
                            return arasy::error::PushFmtError::INCOMPATIBLE_ARG;
                        }
                        break;

                    default:
                        return arasy::error::PushFmtError::INVALID_PLACEHOLDER;
                }
            } while (parsing);

            return checkPushFmt(fmt.substr(idx+1), std::forward(args)...);
        }


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

        template<typename... Args>
        arasy::error::PushFmtError pushFmt(const char *fmt, Args&&... args) {
            auto err = checkPushFmt(std::string_view{fmt}, args...);
            if (err == arasy::error::PushFmtError::NONE) {
                lua_pushfstring(state, fmt, args...);
            }
            return err;
        }

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
