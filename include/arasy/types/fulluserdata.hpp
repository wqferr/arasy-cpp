#pragma once

#include "arasy/types/indexable.hpp"

namespace arasy::core {
    class LuaFullUserData : public internal::LuaIndexable {
    public:
        LuaFullUserData(lua_State* L, int index): LuaIndexable(L, index) {}
    };

    namespace internal {
        template<>
        struct LuaStackReader<LuaFullUserData> {
            static bool checkAt(lua_State* L, int idx) {
                return lua_isuserdata(L, idx) && !lua_islightuserdata(L, idx);
            }

            static std::optional<LuaFullUserData> readAt(lua_State* L, int idx) {
                if (checkAt(L, idx)) {
                    return LuaFullUserData(L, idx);
                } else {
                    return std::nullopt;
                }
            }
        };
    }
}
