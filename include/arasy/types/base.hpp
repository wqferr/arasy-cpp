#pragma once

#include "arasy/lua.hpp"

#include <type_traits>
#include <optional>

namespace arasy::core {
    namespace internal {
        class LuaBaseType {
        public:
            virtual void pushOnto(lua_State* L) const = 0;

            // Moves the value into another Lua state
            virtual void transportTo(lua_State* to) {}
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
    constexpr bool is_lua_wrapper_type_v = std::is_base_of_v<internal::LuaBaseType, T>;

    template<typename T>
    constexpr bool is_nonvariant_lua_wrapper_type_v = std::is_base_of_v<internal::LuaBaseType, T>;

    // template<typename T>
    // constexpr bool is_convertible_to_lua_type_v =
    //     is_lua_wrapper_type_v<T>
    //     || std::is_integral_v<T>
    //     || std::is_floating_point_v<T>
    //     || std::is_same_v<T, const char*>;

    template<typename... Ts>
    constexpr bool all_are_convertible_to_lua_value_v = (std::is_convertible_v<Ts, arasy::core::LuaValue> && ...);

    // template<typename... Ts>
    // struct all_are_lua_wrapper_type {
    //     static constexpr bool value = false;
    // };

    // template<typename T>
    // struct all_are_lua_wrapper_type<std::enable_if_t<is_lua_wrapper_type_v<T>>, T> {
    //     static constexpr bool value = true;
    // };

    // template<typename T, typename... Ts>
    // struct all_are_lua_wrapper_type<std::enable_if_t<is_lua_wrapper_type_v<T>, T>, Ts...> {
    //     static constexpr bool value = all_are_lua_wrapper_type<Ts...>::value;
    // };

    // template<typename... Ts>
    // constexpr bool all_are_lua_wrapper_type_v = all_are_lua_wrapper_type<Ts...>::value;

    // template<typename Arg1>
    // constexpr bool all_are_lua_wrapper_type_v = is_lua_wrapper_type_v<Arg1>;

    // template<typename Arg1, typename Arg2, typename... Args>
    // constexpr bool all_are_lua_wrapper_type_v = is_lua_wrapper_type_v<Arg1> && is_lua_wrapper_type_v<Arg2, Args...>;

    namespace internal {
        template<typename T, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
        struct LuaStackReader {
            static bool checkAt(lua_State *L, int idx);
            static std::optional<T> readAt(lua_State* L, int idx);
        };
    }
}
