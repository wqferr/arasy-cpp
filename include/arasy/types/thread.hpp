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
            const bool finished;
            const int nret;
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
        };

        std::ostream& operator<<(std::ostream& os, const ResumeResult& r);
    }

    class LuaThread : public internal::LuaBaseType {
        std::shared_ptr<Lua> thread_;

    public:
        LuaThread(lua_State* L);
        LuaThread(const LuaThread& other): thread_(other.thread_) {}
        void pushOnto(lua_State* L) const override;

        template<typename... Args, typename = std::enable_if_t<all_are_convertible_to_lua_value_v<Args...>>>
        thread::ResumeResult resume(bool moveRetOver, Lua& L, const Args&... args) {
            (lua().push(LuaValue{args}), ...);

            int nret;
            int status = lua_resume(lua(), L, sizeof...(args), &nret);

            auto doMoveRet = [this, &L, moveRetOver, nret]() {
                lua().ensureStack(nret);
                if (moveRetOver) {
                    for (int i = nret; i >= 1; i--) {
                        L.changeOwnership(*lua().readStack(-i));
                    }
                    lua_xmove(lua(), L, nret);
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
                    return *lua().wrapScriptError(status);
            }
        }

        template<typename... Args, typename = std::enable_if_t<all_are_convertible_to_lua_value_v<Args...>>>
        thread::ResumeResult resumeWith(bool moveRetOver, Lua& L, LuaFunction& f, const Args&... args) {
            push(f);
            return resumeOther(moveRetOver, L, args...);
        }

        Lua& lua() { return *thread_; }
        const Lua& lua() const { return *thread_; }
    };
    bool operator==(const LuaThread& a, const LuaThread& b);

    namespace internal {
        template<>
        struct LuaStackReader<LuaThread> {
            static bool checkAt(lua_State* L, int idx) {
                return lua_iscfunction(L, idx);
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
