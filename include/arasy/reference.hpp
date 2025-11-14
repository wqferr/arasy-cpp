#pragma once

#include <memory>

#include "arasy/types/base.hpp"
#include "arasy/registry.hpp"

namespace arasy::registry {
    class LuaReference : public arasy::core::internal::LuaBaseType {
        int id;
        std::shared_ptr<int> refCount;
        LuaRegistry& registry;

    public:

    };
}
