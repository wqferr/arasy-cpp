#include "arasy/lua.hpp"

#include "arasy/types/base.hpp"

namespace arasy::core {
    class LuaBoolean : public internal::LuaBaseType {
    public:
        const bool value;
        constexpr LuaBoolean(bool value_): value(value_) {}
        constexpr operator bool() const { return value; }
        void pushOnto(lua_State* L) const override { lua_pushboolean(L, value); }
    };
    inline bool operator==(const LuaBoolean& a, const LuaBoolean& b) { return a.value == b.value; }

    /// @brief "true" wrapped in a LuaBoolean.
    constexpr const inline LuaBoolean True {true};

    /// @brief "false" wrapped in a LuaBoolean.
    constexpr const inline LuaBoolean False {false};

    namespace internal {
        template<>
        struct LuaStackReader<LuaBoolean> {
            static bool checkAt(lua_State* L, int idx) {
                return lua_isboolean(L, idx);
            }

            static std::optional<LuaBoolean> readAt(lua_State* L, int idx) {
                return lua_toboolean(L, idx);
            }
        };
    }
}
