#include "lua.hpp"

namespace arasy::core {
    class LuaBoolean {
    public:
        const bool value;
        constexpr LuaBoolean(bool value_): value(value_) {}
        constexpr operator bool() const { return value; }
    };
}
