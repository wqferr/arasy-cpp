#pragma once

#include <cstring>

namespace arasy::core {
    class LuaString : public internal::LuaBaseType {
    public:
        const char *str;
        constexpr LuaString(const char *str_): str(str_) {}
        void pushOnto(lua_State* L) const override { lua_pushstring(L, str); };

        bool operator==(const LuaString& other) const { return strcmp(str, other.str) == 0; }
    };

    namespace internal {
        template<>
        struct LuaStackReader<LuaString> {
            static bool checkAt(lua_State* L, int idx) {
                return lua_isstring(L, idx);
            }

            static std::optional<LuaString> readAt(lua_State* L, int idx) {
                return lua_tostring(L, idx);
            }
        };
    }
}
