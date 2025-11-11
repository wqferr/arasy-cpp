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
}
