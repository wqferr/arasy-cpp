#pragma once

#include <string>

namespace arasy::core {
    class LuaString : public internal::LuaBaseType {
        std::string str_;
    public:
        LuaString(const char *str_): str_(str_) {}
        const char *str() const { return str_.c_str(); }
        void pushOnto(lua_State* L) const override { lua_pushstring(L, str_.c_str()); }

        bool operator==(const LuaString& other) const { return str_ == other.str_; }
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
