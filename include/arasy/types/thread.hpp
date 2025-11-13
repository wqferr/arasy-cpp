#pragma once

#include "arasy/types/base.hpp"

#include <memory>

namespace arasy::core {
    class Lua;

    class LuaThread : public internal::LuaBaseType {
        std::unique_ptr<Lua> thread;

    public:
        LuaThread(lua_State* L): thread(std::make_unique<Lua>(L)) {}
        void pushOnto(lua_State* L) const override;

        operator Lua&() { return *thread; }
    };

    namespace internal {
        template<>
        struct LuaStackReader<LuaThread> {
            static bool checkAt(lua_State* L, int idx) {
                return lua_iscfunction(L, idx);
            }

            static std::optional<LuaThread> readAt(lua_State* L, int idx) {
                if (lua_isthread(L, idx)) {
                    return LuaThread{lua_tothread(L, idx)};
                } else {
                    return std::nullopt;
                }
            }
        };
    }
}
