#include "lua.hpp"

namespace arasy::core {
    class LuaBoolean {
        const bool value;

    public:
        LuaBoolean(bool value_): value(value_) {}
        operator bool() const { return value; }
    };
}
