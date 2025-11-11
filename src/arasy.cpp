#include "arasy.hpp"

using namespace arasy;
using namespace arasy::core;

int Lua::size() const {
    return lua_gettop(state);
}

void Lua::push(const LuaValue& value) {
    value.pushOnto(state);
}

LuaValue Lua::getGlobal(const std::string& name) {
    lua_pushstring(state, name.c_str());
    return pop().value_or(nil);
}




Lua::GlobalVariableProxy& Lua::operator[](const std::string& name) {
    latestVariableAccessed.emplace(*this, name);
    return *latestVariableAccessed;
}

Lua::GlobalVariableProxy::operator LuaValue() const {
    return value();
}

LuaValue Lua::GlobalVariableProxy::value() const {
    lua_getglobal(L, globalName.c_str());
    return L.pop().value_or(nil);
}

Lua::GlobalVariableProxy& Lua::GlobalVariableProxy::operator=(const lua_Number& value) {
    return *this = LuaNumber{value};
}

Lua::GlobalVariableProxy& Lua::GlobalVariableProxy::operator=(const char *str) {
    return *this = LuaString{str};
}
