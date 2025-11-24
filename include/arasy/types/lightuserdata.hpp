#pragma once

#include "arasy/types/base.hpp"

namespace arasy::core {
    class LuaLightUserData : public internal::LuaBaseType {
        void* ptr_;
    public:
        LuaLightUserData(void* ptr__): ptr_(ptr__) {}
        LuaLightUserData(const LuaLightUserData& other): ptr_(other.ptr_) {}
        void pushOnto(lua_State* L) const override { lua_pushlightuserdata(L, ptr_); }

        template<typename P = void*, typename = std::enable_if_t<std::is_pointer_v<P>>>
        constexpr P ptr() const { return static_cast<P>(ptr_); }
    };
    constexpr bool operator==(const LuaLightUserData& a, const LuaLightUserData& b) {
        return a.ptr() == b.ptr();
    }

    namespace internal {
        template<>
        struct LuaStackReader<LuaLightUserData> {
            static bool checkAt(lua_State* L, int idx) {
                return lua_islightuserdata(L, idx);
            }

            static std::optional<LuaLightUserData> readAt(lua_State* L, int idx) {
                if (checkAt(L, idx)) {
                    return lua_touserdata(L, idx);
                } else {
                    return std::nullopt;
                }
            }
        };
    }
}
