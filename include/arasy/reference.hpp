#pragma once

#include <memory>

#include "arasy/types/base.hpp"
#include "arasy/registry.hpp"

namespace arasy::registry {
    class LuaReference : public arasy::core::internal::LuaBaseType {
    protected:
        LuaRegistry registry;
        void pushSelf();

    private:
        std::shared_ptr<char> refCount;
        int id;

        int registerId(lua_State* L, int idx);

    public:
        LuaReference(lua_State* L, int idx);
        virtual ~LuaReference();
        void pushOnto(lua_State* L) const override;

        // LuaReference& operator=(const LuaReference& other);
        // LuaReference& operator=(LuaReference&& other);
        // LuaReference(const LuaReference& other);
        // LuaReference(LuaReference&& other);
    };
}
