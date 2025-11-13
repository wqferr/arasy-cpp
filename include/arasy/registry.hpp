#pragma once

#include "arasy/types/all.hpp"

namespace arasy::core {
    class Lua;

    class LuaRegistry {
        Lua& L;
    public:
        void *const mainKey = new int[0];

        LuaRegistry(Lua& L_):  L(L_) {}
        ~LuaRegistry() { delete[] mainKey; }

        void pushSelf();
        // DO NOT IMPLEMENT getField(int), this is reserved for luaL_ref
        LuaValue getField(const char *fieldName);
        LuaValue getIndex(const LuaValue& key);
    };
}
