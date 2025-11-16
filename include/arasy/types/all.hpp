#pragma once

#include "arasy/types/nil.hpp"
#include "arasy/types/boolean.hpp"
#include "arasy/types/number.hpp"
#include "arasy/types/integer.hpp"
#include "arasy/types/string.hpp"
#include "arasy/types/table.hpp"
#include "arasy/types/function.hpp"
#include "arasy/types/cfunction.hpp"
#include "arasy/types/thread.hpp"

#include "arasy/utils.hpp"

#include <ostream>
#include <variant>

namespace arasy::core {
#define _ARASY_LUA_VARIANT_ORDER LuaNil, LuaBoolean, LuaInteger, LuaNumber, LuaString, LuaTable, LuaFunction, LuaCFunction, LuaThread
    namespace internal {
        using LuaValueVariant = std::variant<_ARASY_LUA_VARIANT_ORDER>;
    }

    enum class LuaValueVarIndex {
        _ARASY_LUA_VARIANT_ORDER
    };

    std::ostream& operator<<(std::ostream& os, const LuaValueVarIndex& value);

    class LuaValue : public internal::LuaValueVariant {
    public:
        using internal::LuaValueVariant::LuaValueVariant;
        using internal::LuaValueVariant::operator=;

        template<typename T, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
        LuaValue(const T& val): internal::LuaValueVariant(val) {}
        LuaValue(const lua_Number& val): internal::LuaValueVariant(LuaNumber{val}) {}
        LuaValue(const std::string& str): internal::LuaValueVariant(LuaString{str.c_str()}) {}

        template<typename T, typename = std::enable_if_t<is_nonvariant_lua_wrapper_type_v<T>>>
        constexpr bool isA() const {
            return std::holds_alternative<T>(*this);
        }

        template<typename T, typename = std::enable_if_t<is_nonvariant_lua_wrapper_type_v<T>>>
        constexpr T& asA() {
            return std::get<T>(*this);
        }

        template<typename T, typename = std::enable_if_t<is_nonvariant_lua_wrapper_type_v<T>>>
        constexpr const T& asA() const {
            return std::get<T>(*this);
        }

        // NOT to be compared against LUA_TNIL and the like.
        constexpr LuaValueVarIndex luaTypeId() const {
            return static_cast<LuaValueVarIndex>(index());
        }

        template<typename R=void, typename C>
        R visit(C&& callable) {
            return std::visit<R>(callable, static_cast<LuaValueVariant&>(*this));
        }

        template<typename R=void, typename C>
        R visit(C&& callable) const {
            return std::visit<R>(
                std::forward<C>(callable),
                static_cast<const LuaValueVariant&>(*this)
            );
        }

        void pushOnto(lua_State* L) const {
            return std::visit(
                [L](const auto& x) { x.pushOnto(L); },
                static_cast<const internal::LuaValueVariant&>(*this)
            );
        }

        std::optional<lua_Number> toNumber() const;
        bool isNumeric() const;
        inline bool isNil() const { return isA<LuaNil>(); }
    };
#undef _ARASY_LUA_VARIANT_ORDER

    template<>
    constexpr const bool is_lua_wrapper_type_v<LuaValue> = true;

    namespace internal {
        template<>
        struct LuaStackReader<LuaValue> {
            static constexpr bool checkAt(lua_State *L, int idx) { return true; }
            static std::optional<LuaValue> readAt(lua_State* L, int idx);
        };
    }

    std::ostream& operator<<(std::ostream& os, const LuaValue& lv);

    constexpr bool operator==(const LuaValue& a, const lua_Number& b) {
        return
            std::holds_alternative<LuaNumber>(a) && std::get<LuaNumber>(a).value == b
            || std::holds_alternative<LuaInteger>(a) && std::get<LuaInteger>(a).value == b;
    }

    constexpr bool operator==(const lua_Number& a, const LuaValue& b) {
        return std::holds_alternative<LuaNumber>(b) && a == std::get<LuaNumber>(b).value
            || std::holds_alternative<LuaInteger>(b) && std::get<LuaInteger>(b).value == a;
    }
}
