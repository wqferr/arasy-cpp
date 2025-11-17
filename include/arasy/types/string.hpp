#pragma once

#include <string>
#include <stdexcept>

namespace arasy::core {
    class LuaString : public internal::LuaBaseType {
        std::string str_;

        const char* checkNullptr(const char* str) {
            if (str == nullptr) {
                throw std::runtime_error("LuaString was given nullptr");
            }
            return str;
        }
    public:
        LuaString(const char *str__): str_(checkNullptr(str__)) {}
        LuaString(const LuaString& s): str_(s.str_) {}
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

namespace arasy::literals {
    inline core::LuaString operator"" _ls(const char* str, std::size_t) {
        return core::LuaString{str};
    }
}
