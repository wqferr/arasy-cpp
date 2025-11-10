#pragma once

#include "types/nil.hpp"
#include "types/boolean.hpp"
#include "types/number.hpp"
#include "types/integer.hpp"
#include "types/string.hpp"
#include "types/cfunction.hpp"

namespace arasy::core {
    namespace types {
        using LuaValue = std::variant<LuaNil, LuaBoolean, LuaInteger, LuaNumber, LuaString, LuaCFunction>;
    }
    using LuaValue = types::LuaValue;
}
