#pragma once

#include "lua.hpp"

#include <concepts>
#include <optional>

namespace arasy::core {
    namespace internal {
        class LuaBaseType {
        public:
            virtual void pushOnto(lua_State* L) const = 0;
        };
    }

    template<typename T>
    concept is_lua_wrapper_type = std::derived_from<T, internal::LuaBaseType>;

    namespace internal {
        template<typename T>
        requires (is_lua_wrapper_type<T>)
        struct LuaStackReader {
            static bool checkAt(lua_State *L, int idx);
            static std::optional<T> readAt(lua_State* L, int idx);
        };
    }
}
