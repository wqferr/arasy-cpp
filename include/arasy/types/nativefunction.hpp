#pragma once
#include "arasy/types/callable.hpp"
#include "arasy/reference.hpp"

namespace arasy::core {
    class LuaNativeFunction : public arasy::registry::LuaReference, public internal::LuaCallable {
    public:
        LuaNativeFunction(lua_State* L, int index): LuaReference(L, index), LuaCallable(L) {}

        void pushOnto(lua_State* L) const override { LuaReference::pushOnto(L); }
    };

    inline bool operator==(const LuaNativeFunction& a, const LuaNativeFunction& b) {
        using LR = arasy::registry::LuaReference;
        return static_cast<const LR&>(a) == static_cast<const LR&>(b);
    }

    namespace internal {
        template<>
        struct LuaStackReader<LuaNativeFunction> {
            static bool checkAt(lua_State* L, int idx) {
                return lua_isfunction(L, idx) && !lua_iscfunction(L, idx);
            }

            static std::optional<LuaNativeFunction> readAt(lua_State* L, int idx) {
                if (checkAt(L, idx)) {
                    return LuaNativeFunction(L, idx);
                } else {
                    return std::nullopt;
                }
            }
        };
    }
}
