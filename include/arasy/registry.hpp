#pragma once

#include <optional>

#include "arasy/types/all.hpp"
#include "lua.hpp"

namespace arasy::registry {
    class LuaRegistry {
        lua_State* const L;

    public:
        LuaRegistry(lua_State* L_);

        // DO NOT IMPLEMENT readField(int), this is reserved for luaL_ref
        arasy::core::LuaValue readField(const char* fieldName);
        void retrieveField(const char* fieldName);

        template<typename T, typename = std::enable_if_t<is_nonvariant_lua_wrapper_type_v<T>>>
        std::optional<T> readField(const char* fieldName) {
            LuaValue value = readField(fieldName);
            if (value.isA<T>()) {
                return value.asA<T>();
            } else {
                return std::nullopt;
            }
        }

        arasy::core::LuaValue readKey(const arasy::core::LuaValue& key);

        template<typename T, typename = std::enable_if_t<is_nonvariant_lua_wrapper_type_v<T>>>
        std::optional<T> readKey(const T& key) {
            LuaValue value = readKey(key);
            if (value.isA<T>()) {
                return value.asA<T>();
            } else {
                return std::nullopt;
            }
        }

        void retrieve(const arasy::core::LuaValue& key);

        void writeField(const char* fieldName, const arasy::core::LuaValue& value);
        void storeField(const char* fieldName);
        void writeKey(const arasy::core::LuaValue& key, const arasy::core::LuaValue& value);
        void storeKey(const arasy::core::LuaValue& key);

        void retrieveRef(int ref);
    };
}
