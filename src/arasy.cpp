#include "arasy.hpp"

using namespace arasy;
using namespace arasy::core;

int Lua::size() const {
    return lua_gettop(state);
}

void Lua::push(const LuaValue& value) {
    std::visit(
        [this](const auto& v) {
            v.pushOnto(this->state);
        },
        value
    );
}
