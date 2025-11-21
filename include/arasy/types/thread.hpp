#pragma once

#include "arasy/types/base.hpp"
#include "arasy/types/function.hpp"
#include "arasy/errors.hpp"

#include <memory>
#include <variant>

namespace arasy::core {
    class Lua;

    namespace thread {
        struct Ok {
            bool finished;
            int nret;
        };

        class ResumeResult : std::variant<Ok, error::ScriptError> {
        public:
            ResumeResult(const Ok& ok): variant(ok) {}
            ResumeResult(const error::ScriptError& error): variant(error) {}

            bool isError() const { return std::holds_alternative<error::ScriptError>(*this); }
            const error::ScriptError& error() const { return std::get<error::ScriptError>(*this); }

            bool isOk() const { return std::holds_alternative<Ok>(*this); }
            const Ok& value() const { return std::get<Ok>(*this); }

            Ok* operator->() { return std::get_if<Ok>(this); }
            const Ok* operator->() const { return std::get_if<Ok>(this); }

            operator int() const {
                if (isOk()) {
                    return value().finished ? LUA_OK : LUA_YIELD;
                } else {
                    return static_cast<int>(error().code);
                }
            }
        };

        std::ostream& operator<<(std::ostream& os, const ResumeResult& r);
    }

    class LuaThread : public internal::LuaBaseType {
        lua_State* thread_;

    public:
        LuaThread(lua_State* L);
        LuaThread(const LuaThread& other): thread_(other.thread_) {}
        void pushOnto(lua_State* L) const override;

        template<typename... Args, typename = std::enable_if_t<all_are_lua_types_v<Args...>>>
        thread::ResumeResult resume(bool moveRetOver, lua_State* L, const Args&... args) {
            (args.pushOnto(thread_), ...);

            int nret;
            int status = lua_resume(thread_, L, sizeof...(args), &nret);

            auto doMoveRet = [this, &L, moveRetOver, nret]() {
                lua_checkstack(thread_, nret);
                if (moveRetOver) {
                    lua_xmove(thread_, L, nret);
                }
            };
            switch (status) {
                case LUA_YIELD:
                    doMoveRet();
                    return thread::Ok({ false, nret });
                case LUA_OK:
                    doMoveRet();
                    return thread::Ok({ true, nret });
                default:
                    return *error::wrapScriptError(thread_, status);
            }
        }

        template<typename... Args, typename = std::enable_if_t<all_are_convertible_to_lua_value_v<Args...>>>
        thread::ResumeResult start(bool moveRetOver, lua_State* L, LuaFunction& f, const Args&... args) {
            f.pushOnto(thread_);
            return resume(moveRetOver, L, args...);
        }

        template<typename... Args, typename = std::enable_if_t<all_are_convertible_to_lua_value_v<Args...>>>
        thread::ResumeResult start(bool moveRetOver, lua_State* L, lua_CFunction f, const Args&... args) {
            lua_pushcfunction(thread_, f);
            return resume(moveRetOver, L, args...);
        }

        lua_State* lua() { return thread_; }
        const lua_State* lua() const { return thread_; }
    };
    bool operator==(const LuaThread& a, const LuaThread& b);

    namespace internal {
        template<>
        struct LuaStackReader<LuaThread> {
            static bool checkAt(lua_State* L, int idx) {
                return lua_isthread(L, idx);
            }

            static std::optional<LuaThread> readAt(lua_State* L, int idx) {
                if (lua_isthread(L, idx)) {
                    return std::optional<LuaThread>{lua_tothread(L, idx)};
                } else {
                    return std::nullopt;
                }
            }
        };
    }
}
