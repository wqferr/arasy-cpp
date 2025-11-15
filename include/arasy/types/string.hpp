#pragma once

#include <string>

namespace arasy::core {
    class LuaString : public internal::LuaBaseType {
        std::string str_;
    public:
        LuaString(const char *str_): str_(str_) {}
        const char *str() const { return str_.c_str(); }
        const std::string& fullStr() const { return str_; }
        void pushOnto(lua_State* L) const override { lua_pushstring(L, str_.c_str()); }
    };

    inline bool operator==(const LuaString& a, const LuaString& b) { return a.fullStr() == b.fullStr(); }

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
