#pragma once

#include <type_traits>

#include "types/base.hpp"

namespace arasy::core {
    template<typename T>
    struct is_lua_wrapper_type {
        static constexpr bool value = std::is_base_of_v<LuaBaseType, T>;
    };
}
