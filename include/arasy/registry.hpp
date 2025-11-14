#pragma once

#include "arasy/types/all.hpp"
#include "lua.hpp"

namespace arasy::registry {
    class LuaRegistry {
        lua_State* const L;

    public:
        LuaRegistry(lua_State* L_);

        // DO NOT IMPLEMENT readField(int), this is reserved for luaL_ref
        arasy::core::LuaValue readField(const char* fieldName);
        void retrieveField(const char* fieldName);
        arasy::core::LuaValue readKey(const arasy::core::LuaValue& key);
        void retrieve(const arasy::core::LuaValue& key);

        void writeField(const char* fieldName, const arasy::core::LuaValue& value);
        void storeField(const char* fieldName);
        void writeKey(const arasy::core::LuaValue& key, const arasy::core::LuaValue& value);
        void storeKey(const arasy::core::LuaValue& key);

        void retrieveRef(int ref);
    };
}
