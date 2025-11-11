#pragma once

#include "types/nil.hpp"
#include "types/boolean.hpp"
#include "types/number.hpp"
#include "types/integer.hpp"
#include "types/string.hpp"
#include "types/cfunction.hpp"

#include <ostream>
#include <variant>

namespace arasy::core {
    using LuaValue = std::variant<LuaNil, LuaBoolean, LuaInteger, LuaNumber, LuaString, LuaCFunction>;

    std::ostream& operator<<(std::ostream& os, const LuaValue& lv);

    template<typename T, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
    bool operator==(const T& a, const LuaValue& b) {
        return std::holds_alternative<T>(b) && a == std::get<T>(b);
    }

    template<typename T, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
    bool operator==(const LuaValue& a, const T& b) {
        return std::holds_alternative<T>(a) && std::get<T>(a) == b;
    }
}
