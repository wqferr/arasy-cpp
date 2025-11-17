#pragma once

#include "arasy/types/base.hpp"
#include "arasy/registry.hpp"

namespace arasy::registry {
    class LuaReference : public arasy::core::internal::LuaBaseType {
    protected:
        LuaRegistry registry;
        void pushSelf() const;

        LuaReference(lua_State* L);
        void doRegister();

        bool fromSameThreadTreeAs(lua_State* otherLua) const;

    private:
        int id_;
        int makeId(int idx);
        int clone(const LuaReference& ref);

    public:
        LuaReference(lua_State* L, int idx);

        virtual ~LuaReference();
        LuaReference(const LuaReference& other);
        LuaReference(LuaReference&& other);
        LuaReference& operator=(const LuaReference& other);
        LuaReference& operator=(LuaReference&& other);

        void pushOnto(lua_State* L) const override;
        bool operator==(const LuaReference& other) const;

        int id() const { return id_; }
        void transportTo(lua_State* L) override;
    };
}
