#include "lua.hpp"

#include "types/base.hpp"

namespace arasy::core {
    class LuaBoolean : public internal::LuaBaseType {
    public:
        const bool value;
        constexpr LuaBoolean(bool value_): value(value_) {}
        constexpr operator bool() const { return value; }
        void pushOnto(lua_State* L) const override { lua_pushboolean(L, value); };
    };

    namespace internal {
        template<>
        struct LuaStackReader<LuaBoolean> {
            static bool checkAt(lua_State* L, int idx) {
                return lua_isstring(L, idx);
            }

            static std::optional<LuaBoolean> readAt(lua_State* L, int idx) {
                return lua_toboolean(L, idx);
            }
        };
    }
}
