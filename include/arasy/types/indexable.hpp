#pragma once

#include "arasy/errors.hpp"
#include "arasy/types/callable.hpp"
#include "arasy/types/integer.hpp"

namespace arasy::core {
    class LuaValue;
    class LuaTable;
}

namespace arasy::core::internal {
    class LuaIndexable : public LuaCallable {
    public:
        LuaIndexable(lua_State* L, int idx): LuaCallable(L, idx) {}

        std::optional<arasy::error::IndexingError> setStackKV();
        std::optional<arasy::error::IndexingError> set(const LuaValue& key, const LuaValue& value);
        std::optional<arasy::error::IndexingError> setField(const char* fieldName, const LuaValue& value);

        std::optional<arasy::error::IndexingError> setRawStackKV();
        std::optional<arasy::error::IndexingError> setRaw(const LuaValue& key, const LuaValue& value);
        std::optional<arasy::error::IndexingError> setRawi(const LuaInteger& i, const LuaValue& value);

        template<typename T = LuaValue, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
        std::optional<T> get(const LuaValue& key) {
            lua_State* L = registry.luaInstance;
            if (auto err = retrieve(key)) {
                return std::nullopt;
            }
            auto ret = arasy::core::internal::LuaStackReader<T>::readAt(L, -1);
            lua_pop(L, 1);
            return ret;
        }

        template<typename T = LuaValue, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
        std::optional<T> get(const LuaValue& key, std::optional<arasy::error::IndexingError>& err) {
            lua_State* L = registry.luaInstance;
            if (err = retrieve(key)) {
                return std::nullopt;
            }
            auto ret = arasy::core::internal::LuaStackReader<T>::readAt(L, -1);
            lua_pop(L, 1);
            return ret;
        }

        template<typename T = LuaValue, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
        std::optional<T> getField(const char* fieldName) {
            lua_State* L = registry.luaInstance;
            retrieveField(fieldName);
            auto ret = arasy::core::internal::LuaStackReader<T>::readAt(L, -1);
            lua_pop(L, 1);
            return ret;
        }

        std::optional<arasy::error::IndexingError> retrieve(const LuaValue& key);
        void retrieveField(const char* fieldName);

        void setMetatableStack();
        void setMetatable(const LuaTable& metatable);

        // TODO
        struct OperationChain {};
        OperationChain chain();
    };
}
