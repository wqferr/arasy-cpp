#pragma once

#include "arasy/types/base.hpp"
#include "arasy/registry/reference.hpp"
#include "arasy/errors.hpp"

namespace arasy::core {
    class LuaValue;

    class LuaTable : public arasy::registry::LuaReference {
    public:
        LuaTable(lua_State* L, int index): LuaReference(L, index) {}
        std::optional<arasy::error::TableIndexingError> setStackKV();
        std::optional<arasy::error::TableIndexingError> set(const LuaValue& key, const LuaValue& value);
        std::optional<arasy::error::TableIndexingError> setField(const char* fieldName, const LuaValue& value);

        void setRawStackKV();
        void setRaw(const LuaValue& key, const LuaValue& value);
        void setRawField(const char* fieldName, const LuaValue& value);
        void setRawi(const lua_Integer& i, const LuaValue& value);

        template<typename T = LuaValue, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
        std::optional<T> get(const LuaValue& key) {
            lua_State* L = registry.luaInstance;
            if (auto err = index(key)) {
                return std::nullopt;
            }
            auto ret = arasy::core::internal::LuaStackReader<T>::readAt(L, -1);
            lua_pop(L, 1);
            return ret;
        }

        template<typename T = LuaValue, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
        std::optional<T> get(const LuaValue& key, std::optional<arasy::error::TableIndexingError>& err) {
            lua_State* L = registry.luaInstance;
            if (err = index(key)) {
                return std::nullopt;
            }
            auto ret = arasy::core::internal::LuaStackReader<T>::readAt(L, -1);
            lua_pop(L, 1);
            return ret;
        }

        template<typename T = LuaValue, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
        std::optional<T> getField(const char* fieldName) {
            lua_State* L = registry.luaInstance;
            indexField(fieldName);
            auto ret = arasy::core::internal::LuaStackReader<T>::readAt(L, -1);
            lua_pop(L, 1);
            return ret;
        }

        std::optional<arasy::error::TableIndexingError> index(const LuaValue& key);
        void indexField(const char* fieldName);

        void setMetatableStack();
        void setMetatable(const LuaTable& metatable);

        // TODO
        struct OperationChain {};
        OperationChain chain();

        // bool operator==(const LuaTable& other) {
        //     return LuaReference::operator==(other);
        // }
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
