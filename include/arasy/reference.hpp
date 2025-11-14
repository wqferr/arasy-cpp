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
        int makeId(int idx);
        int refCount();
        int id_;

    public:
        LuaReference(lua_State* L, int idx);
        virtual ~LuaReference();
        void pushOnto(lua_State* L) const override;
        bool operator==(const LuaReference& other) const { return id_ == other.id_; }

        int id() const { return id_; }
    };
}
