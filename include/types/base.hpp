#pragma once

#include "lua.hpp"

#include <type_traits>

namespace arasy::core {
    namespace internal {
        class LuaBaseType {
        public:
            virtual void pushOnto(lua_State* L) const = 0;
        };
    }

    template<typename T>
    struct is_lua_wrapper_type {
        static constexpr bool value = std::is_base_of_v<internal::LuaBaseType, T>;
    };

    template<typename T>
    constexpr const inline bool is_lua_wrapper_type_v = is_lua_wrapper_type<T>::value;

    template<typename T, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
    struct LuaStackReader {
        static std::optional<T> readAt(lua_State* L, int idx);
    };
}
