#pragma once

#include "arasy/types/base.hpp"
#include "arasy/reference.hpp"
#include "arasy/errors.hpp"

namespace arasy::core {
    class LuaTable : public arasy::registry::LuaReference {
    public:
        LuaTable(lua_State* L, int index): LuaReference(L, index) {}
        std::optional<arasy::error::TableIndexingErrorCode> setStackKV();
        std::optional<arasy::error::TableIndexingErrorCode> set(const LuaValue& key, const LuaValue& value);
        std::optional<arasy::error::TableIndexingErrorCode> setField(const char* fieldName, const LuaValue& value);
        void seti(const lua_Integer& i, const lua_Integer& value);

        void setRawStackKV();
        void setRaw(const LuaValue& key, const LuaValue& value);
        void setRawField(const char* fieldName, const LuaValue& value);
        void setRawi(const lua_Integer& i, const lua_Integer& value);

        LuaValue get(const LuaValue& key);
        LuaValue getField(const char* fieldName);
        LuaValue geti(const lua_Integer& i);

        LuaValue index(const LuaValue& key);
        LuaValue indexField(const char* fieldName);
        LuaValue indexi(const lua_Integer& i);

        void setMetatableStack();
        void setMetatable(const LuaTable& metatable);
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
