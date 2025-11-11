#pragma once

#include "types/nil.hpp"
#include "types/boolean.hpp"
#include "types/number.hpp"
#include "types/integer.hpp"
#include "types/string.hpp"
#include "types/cfunction.hpp"

#include "utils.hpp"

#include <ostream>
#include <variant>

namespace arasy::core {
    using LuaValue = std::variant<LuaNil, LuaBoolean, LuaInteger, LuaNumber, LuaString, LuaCFunction>;

    template<>
    constexpr const bool is_lua_wrapper_type_v<LuaValue> = true;

    namespace internal {
        template<>
        struct LuaStackReader<LuaValue> {
            static constexpr bool checkAt(lua_State *L, int idx) { return true; }
            static std::optional<LuaValue> readAt(lua_State* L, int idx);
        };
    }

    std::ostream& operator<<(std::ostream& os, const LuaValue& lv);

    template<typename T, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
    bool operator==(const LuaValue& a, const T& b) {
        return std::holds_alternative<T>(a) && std::get<T>(a) == b;
    }

    template<typename T, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
    bool operator==(const T& a, const LuaValue& b) {
        return std::holds_alternative<T>(b) && a == std::get<T>(b);
    }

    constexpr bool operator==(const LuaValue& a, const lua_Number& b) {
        return
            std::holds_alternative<LuaNumber>(a) && std::get<LuaNumber>(a).value == b
            || std::holds_alternative<LuaInteger>(a) && std::get<LuaInteger>(a).value == b;;
    }

    constexpr bool operator==(const lua_Number& a, const LuaValue& b) {
        return std::holds_alternative<LuaNumber>(b) && a == std::get<LuaNumber>(b).value
            || std::holds_alternative<LuaInteger>(b) && std::get<LuaInteger>(b).value == a;
    }
}
