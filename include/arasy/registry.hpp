#pragma once

#include "arasy/types/all.hpp"

namespace arasy::core {
    class Lua;
}
namespace arasy::registry {
    class LuaRegistry {
        arasy::core::Lua& L;
    public:
        void *const mainKey = new char;

        LuaRegistry(arasy::core::Lua& L_):  L(L_) {}
        ~LuaRegistry() { delete mainKey; }

        void pushSelf();

        // DO NOT IMPLEMENT getField(int), this is reserved for luaL_ref
        arasy::core::LuaValue getField(const char *fieldName);
        arasy::core::LuaValue getIndex(const arasy::core::LuaValue& key);
        void setField(const char *fieldName, const arasy::core::LuaValue& value);
        void setIndex(const arasy::core::LuaValue& key, const arasy::core::LuaValue& value);
    };
}
