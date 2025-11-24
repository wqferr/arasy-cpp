#pragma once

#include <string_view>
#include <optional>

#include "arasy/types/integer.hpp"
#include "arasy/types/number.hpp"
#include "arasy/types/string.hpp"
#include "arasy/errors.hpp"


namespace arasy::utils::internal {
    template<typename... Args>
    error::MPushFmtError checkPushFmt(const std::string_view fmt) {
        using namespace std::string_literals;

        auto idx = fmt.find('%');
        if (idx == std::string_view::npos) {
            return std::nullopt;
        }
        idx = idx + 1;
        if (idx >= fmt.size()) {
            return arasy::error::PushFmtError{
                arasy::error::PushFmtErrorCode::INVALID_SPECIFIER,
                "Incomplete specifier at end of string"
            };
        } else if (fmt.at(idx) != '%') {
            return arasy::error::PushFmtError{
                arasy::error::PushFmtErrorCode::TOO_FEW_ARGS,
                "Extra specifier at index "s + std::to_string(idx) + ": %" + std::to_string(fmt.at(idx))
            };
        } else {
            return checkPushFmt(fmt.substr(idx+1));
        }
    }

    namespace internal {
        template<typename T>
        [[nodiscard]] arasy::error::PushFmtError incompatArg(const std::string_view fmt, std::size_t idx) {
            using namespace std::string_literals;

            char sp = fmt.at(idx);
            return arasy::error::PushFmtError{
                arasy::error::PushFmtErrorCode::INCOMPATIBLE_ARG,
                "Incompatible arg to given specifier: %"s + std::to_string(sp) + " and " + typeid(T).name()
            };
        }
    }

    template<typename T1, typename... Args>
    error::MPushFmtError checkPushFmt(const std::string_view fmt, T1& firstArg, Args&... args) {
        using namespace std::string_literals;

        auto idx = fmt.find('%');
        if (idx == std::string_view::npos) {
            return arasy::error::PushFmtError{
                arasy::error::PushFmtErrorCode::TOO_MANY_ARGS,
                "Not enough specifiers for the number of arguments given"
            };
        }

        idx++;
        if (idx >= fmt.size()) {
            return arasy::error::PushFmtError{
                arasy::error::PushFmtErrorCode::INVALID_SPECIFIER,
                "Incomplete specifier at end of string"
            };
        }

        switch (fmt.at(idx)) {
            case '%':
                return checkPushFmt(fmt.substr(idx+1), firstArg, args...);

            case 'p':
                if constexpr (!std::is_convertible_v<T1, void*>) {
                    return internal::incompatArg<T1>(fmt, idx);
                }
                break;

            case 'd':
            case 'c':
                if constexpr (!std::is_convertible_v<T1, core::LuaInteger>) {
                    return internal::incompatArg<T1>(fmt, idx);
                }
                break;

            case 'f':
                if constexpr (!std::is_convertible_v<T1, core::LuaNumber>) {
                    return internal::incompatArg<T1>(fmt, idx);
                }
                break;

            case 's':
                if constexpr (!std::is_convertible_v<T1, core::LuaString>) {
                    return internal::incompatArg<T1>(fmt, idx);
                }
                break;

            default:
                return arasy::error::PushFmtError{
                    arasy::error::PushFmtErrorCode::INVALID_SPECIFIER,
                    "Invalid specifier at index "s + std::to_string(idx) + ": %" + std::to_string(fmt.at(idx))
                };
        }

        return checkPushFmt(fmt.substr(idx+1), args...);
    }
}
