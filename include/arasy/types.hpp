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
#include "arasy/types/lightuserdata.hpp"
#include "arasy/types/fulluserdata.hpp"

#include "arasy/utils.hpp"

#include <ostream>
#include <variant>

namespace arasy::core {
#define _ARASY_LUA_VARIANT_ORDER LuaNil, LuaBoolean, LuaInteger, LuaNumber, LuaString, LuaTable, LuaFunction, LuaCFunction, LuaThread, LuaLightUserData, LuaFullUserData
    namespace internal {
        using LuaValueVariant = std::variant<_ARASY_LUA_VARIANT_ORDER>;
    }

    enum class LuaValueType {
        _ARASY_LUA_VARIANT_ORDER
    };

    std::ostream& operator<<(std::ostream& os, const LuaValueType& value);

    class LuaValue;

    template<typename T>
    constexpr bool is_lua_value_v = utils::internal::is_any_of_v<T, LuaValue, _ARASY_LUA_VARIANT_ORDER>;

    /**
     * @brief Variant that holds any valid Lua value.
     *
     * Variants are type-safe unions, so no additional memory allocations are made,
     * and all LuaValues are safely copiable.
     *
     * To check if a LuaValue v holds a particular type (e.g., a LuaInteger),
     * simply call v.isA<LuaInteger>().
     * If you are sure v is indeed a LuaInteger and want to cast it, call
     * v.asA<LuaInteger>() and you will get a LuaInteger on the return value.
     *
     * A special case is v.asA<LuaNumber>(), which will, if v holds a LuaInteger,
     * cast it to a LuaNumber.
     *
     * If you want to switch() over the type of a LuaValue v, you can use v.luaTypeId(),
     * which returns a value from the LuaValueType enum.
     *
     * If you instead want to use the variant interface of visit(), you can either use
     * the std::visit function as for normal variants, or you can use v.visit.
     *
     * int, float, and string iterals can be suffixed with _lv to cast them to LuaValue
     * after using namespace arasy::literals. int and float literals can be suffixed with
     * _ln to cast them to LuaNumber, int literals can be suffixed with _li to cast them
     * to LuaInteger, and string literals can be suffixed with _ls to cast them to LuaString.
     *
     * Due to ambiguity between false and 0, the special values True and False (upper case
     * first letters) are provided as well, to unambiguously represent LuaBooleans. True_lv
     * and False_lv are the same as True and False, except cast into a LuaValue.
     */
    class LuaValue : public internal::LuaValueVariant {
    public:
        using internal::LuaValueVariant::LuaValueVariant;
        using internal::LuaValueVariant::operator=;

        template<typename T, typename = std::enable_if_t<is_lua_value_v<T>>>
        LuaValue(const T& val): internal::LuaValueVariant(val) {}
        LuaValue(const lua_Number& val): internal::LuaValueVariant(LuaNumber{val}) {}
        LuaValue(const std::string& str): internal::LuaValueVariant(LuaString{str.c_str()}) {}

        LuaValue& operator=(const LuaValue& other);

        /**
         * @brief Check if LuaValue holds a particular Lua type.
         *
         * A special case for this function is if you call isA<LuaNumber>() and
         * the variant contains a LuaInteger, if will return true even though the
         * variant types are different.
         *
         * @tparam T type to check against.
         * @return true this LuaValue contains given type T.
         * @return false this LuaValue does not contain given type T.
         */
        template<typename T, typename = std::enable_if_t<is_lua_value_v<T>>>
        constexpr bool isA() const {
            if constexpr (std::is_same_v<T, LuaNumber>) {
                if (isA<LuaInteger>()) {
                    return true;
                }
            }
            return std::holds_alternative<T>(*this);
        }

        /**
         * @brief Cast LuaValue into a specfic Lua type.
         *
         * A special case for this function is if you call asA<LuaNumber>() and
         * the variant contains a LuaInteger, it will be automatically cast for you.
         *
         * @tparam T specific type to cast to.
         * @return T Lua value contained within the variant.
         *
         * @throws std::bad_variant_access
         * If the given type does not match the LuaValue content.
         */
        template<typename T, typename = std::enable_if_t<is_lua_value_v<T>>>
        constexpr T asA() const noexcept(false) {
            if constexpr (std::is_same_v<T, LuaNumber>) {
                if (isA<LuaInteger>()) {
                    return LuaNumber{static_cast<lua_Number>(asA<LuaInteger>().value)};
                }
            }
            return std::get<T>(*this);
        }

        /**
         * @brief Get Arasy-specific numerical identification of Lua value types.
         *
         * This should NOT be compared agaisnt LUA_TNIL, LUA_TTABLE and the like.
         *
         * @return LuaValueType numeric id of the type contained within the variant.
         */
        constexpr LuaValueType luaTypeId() const {
            return static_cast<LuaValueType>(index());
        }

        /**
         * @brief Call std::visit on self.
         *
         * For more information, see https://en.cppreference.com/w/cpp/utility/variant/visit2.html
         *
         * @tparam R Return value of the callable.
         * @param callable The callable to be used.
         * @return R
         */
        template<typename R=void, typename C>
        R visit(C&& callable) {
            if constexpr (std::is_same_v<R, void>) {
                std::visit(callable, *this);
            } else {
                return std::visit<R>(callable, *this);
            }
        }

        /**
         * @brief Call std::visit on self.
         *
         * For more information, see https://en.cppreference.com/w/cpp/utility/variant/visit2.html
         *
         * @tparam R Return value of the callable.
         * @param callable The callable to be used.
         * @return R
         */
        template<typename R=void, typename C>
        R visit(C&& callable) const {
            return std::visit<R>(
                std::forward<C>(callable),
                static_cast<const LuaValueVariant&>(*this)
            );
        }

        /**
         * @brief Push itself onto the lua_State L's stack.
         *
         * @param L Lua state onto which to push self.
         */
        void pushOnto(lua_State* L) const {
            return std::visit(
                [L](const internal::LuaBaseType& x) { x.pushOnto(L); },
                static_cast<const internal::LuaValueVariant&>(*this)
            );
        }

        /**
         * @brief Get the numerical value of a LuaValue; returns std::nullopt if not a LuaNumber or a LuaInteger.
         *
         * @return std::optional<lua_Number> Numerical value of self, if any.
         */
        std::optional<lua_Number> toNumber() const;
        bool isNumeric() const;
        constexpr bool isNil() const { return isA<LuaNil>(); }

        LuaValue operator-() const noexcept(false) {
            if (isNumeric()) {
                return -asA<LuaNumber>();
            }
            throw std::runtime_error("Tried to unary-minus a non-numerical value");
        }
    };

    template<typename... Ts>
    constexpr bool all_are_convertible_to_lua_value_v = (std::is_convertible_v<Ts, arasy::core::LuaValue> && ...);

#undef _ARASY_LUA_VARIANT_ORDER

    template<>
    constexpr const bool is_lua_wrapper_type_v<LuaValue> = true;

    namespace internal {
        template<>
        struct LuaStackReader<LuaValue> {
            static constexpr bool checkAt(lua_State *L, int idx);
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

namespace arasy::literals {
    inline core::LuaValue operator"" _lv (unsigned long long int value) {
        return core::LuaValue{core::LuaInteger{static_cast<lua_Integer>(value)}};
    }

    inline core::LuaValue operator"" _lv(const char* str, std::size_t) {
        return core::LuaValue{core::LuaString{str}};
    }

    inline core::LuaValue operator"" _lv(long double value) {
        return core::LuaValue{core::LuaNumber{static_cast<lua_Number>(value)}};
    }
}

namespace arasy::core {
    const inline LuaValue nil_lv {nil};
    const inline LuaValue True_lv {True};
    const inline LuaValue False_lv {False};
}
