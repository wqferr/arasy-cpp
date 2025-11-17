#pragma once

#include "arasy/types/base.hpp"
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

        struct Error {
            const std::string message;
        };

        class ResumeResult : std::variant<Ok, Error> {
        public:
            ResumeResult(const Ok& ok): variant(ok) {}
            ResumeResult(const Error& error): variant(error) {}

            bool isError() { return std::holds_alternative<Error>(*this); }
            const Error& error() const { return std::get<Error>(*this); }

            bool isOk() { return std::holds_alternative<Ok>(*this); }
            const Ok& value() const { return std::get<Ok>(*this); }
        };
    }

    class LuaThread : public internal::LuaBaseType {
        std::shared_ptr<Lua> thread_;

    public:
        LuaThread(lua_State* L);
        LuaThread(const LuaThread& other): thread_(other.thread_) {}
        void pushOnto(lua_State* L) const override;

        Lua& thread() { return *thread_; }
        const Lua& thread() const { return *thread_; }

        operator Lua&() { return *thread_; }
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
