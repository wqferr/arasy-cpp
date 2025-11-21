#pragma once

#include <optional>

#include "lua.hpp"
#include "arasy/types/base.hpp"
#include "arasy/errors.hpp"

namespace arasy::core {
    class LuaValue;
}

namespace arasy::registry {
    class LuaRegistry {
    public:
        lua_State* const luaInstance;
        LuaRegistry(lua_State* L_);

        // DO NOT IMPLEMENT readField(int), this is reserved for luaL_ref
        void retrieveField(const char* fieldName) const;

        template<typename M>
        error::MLoadModuleError loadModule(const char* name, bool overwriteNameConflicts) {
            int top = lua_gettop(luaInstance);
            lua_getglobal(luaInstance, "package"); // +1
            lua_getfield(luaInstance, -1, "loaded"); // +1
            lua_getfield(luaInstance, -1, name); // +1
            if (lua_isnil(luaInstance, -1) || overwriteNameConflicts) { // +0
                lua_pop(luaInstance, 1); // -1
                M* mod = static_cast<M*>(lua_newuserdata(luaInstance, sizeof(M))); // +1
                lua_pushvalue(luaInstance, -1);
                new (mod) M {luaInstance, name};
                int ref = luaL_ref(luaInstance, LUA_REGISTRYINDEX);
                mod->setRef(ref);
                lua_setfield(luaInstance, -2, name); // -1
                lua_settop(luaInstance, top);
                return error::none;
            } else {
                lua_settop(luaInstance, top);
                return {error::LoadModuleErrorCode::DUPLICATE_NAME};
            }
        }

        template<typename T = arasy::core::LuaValue, typename = std::enable_if_t<arasy::core::is_lua_wrapper_type_v<T>>>
        std::optional<T> readField(const char* fieldName) const {
            using SR = arasy::core::internal::LuaStackReader<T>;

            retrieveField(fieldName);
            auto value = SR::readAt(luaInstance, -1);
            lua_pop(luaInstance, 1);
            return value;
        }

        template<typename T = arasy::core::LuaValue, typename = std::enable_if_t<arasy::core::is_lua_wrapper_type_v<T>>>
        std::optional<T> readKey(const T& key) const {
            using SR = arasy::core::internal::LuaStackReader<T>;

            retrieveKey(key);
            auto value = SR::readAt(luaInstance, -1);
            lua_pop(luaInstance, 1);
            return value;
        }

        void retrieveKey(const arasy::core::LuaValue& key) const;
        void retrieveStack() const;

        void writeField(const char* fieldName, const arasy::core::LuaValue& value);
        void storeField(const char* fieldName);
        void writeKey(const arasy::core::LuaValue& key, const arasy::core::LuaValue& value);
        void storeKey(const arasy::core::LuaValue& key);

        int createRef(int idx);
        void retrieveRef(int ref) const;
        void releaseRef(int ref);
    };
}
