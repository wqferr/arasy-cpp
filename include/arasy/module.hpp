#pragma once

#include "arasy/types.hpp"

namespace arasy::utils {
    class Module {
    public:
        struct Field {
            const char* name;
            Module& parent;
            mutable core::LuaValue cachedValue;

            Field(Module& parent_, const char* name_): parent(parent_), name(name_) {}

            core::LuaValue value() const {
                return *parent.members.getField(name);
            }

            core::LuaValue* operator->() {
                cachedValue = value();
                return &cachedValue;
            }

            const core::LuaValue* operator->() const {
                cachedValue = value();
                return &cachedValue;
            }
        };

        const char* name;
    protected:
        core::LuaTable members;

        Field field(const char* name, core::LuaValue v);

        lua_State* init(lua_State* L) {

            return L;
        }
    };
}
