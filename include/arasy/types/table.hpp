#pragma once

#include "arasy/types/indexable.hpp"

namespace arasy::core {
    class LuaTable : public arasy::core::internal::LuaIndexable {
    public:
        LuaTable(lua_State* L, int index): LuaIndexable(L, index) {}

        void insert(const LuaInteger& position, const LuaValue& value);
        void insert(const LuaValue& value);
    };

    namespace internal {
        template<>
        struct LuaStackReader<LuaTable> {
            static bool checkAt(lua_State* L, int idx) {
                return lua_istable(L, idx);
            }

            static std::optional<LuaTable> readAt(lua_State* L, int idx) {
                if (lua_istable(L, idx)) {
                    return LuaTable(L, idx);
                } else {
                    return std::nullopt;
                }
            }
        };
    }
}
