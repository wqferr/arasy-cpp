#pragma once

#include "arasy/lua.hpp"

#include <type_traits>
#include <optional>

namespace arasy::core {
    namespace internal {
        class LuaBaseType {
        public:
            virtual void pushOnto(lua_State* L) const = 0;
        };
    }

    template<typename T1, typename T2, typename = std::enable_if_t<
        !std::is_same_v<T1, T2>
        && std::is_base_of_v<internal::LuaBaseType, T1>
        && std::is_base_of_v<internal::LuaBaseType, T2>>>
    bool operator==(const T1& a, const T2& b) {
        return false;
    }


    template<typename T>
    constexpr const inline bool is_lua_wrapper_type_v = std::is_base_of_v<internal::LuaBaseType, T>;

    template<typename T>
    constexpr const bool is_nonvariant_lua_wrapper_type_v = std::is_base_of_v<internal::LuaBaseType, T>;

    template<typename T>
    constexpr const bool is_potentially_callable_v = false;

    namespace internal {
        template<typename T, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
        struct LuaStackReader {
            static bool checkAt(lua_State *L, int idx);
            static std::optional<T> readAt(lua_State* L, int idx);
        };
    }
}
