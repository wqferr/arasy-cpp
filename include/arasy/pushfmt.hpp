#pragma once

#include <string_view>
#include <optional>

#include "arasy/errors.hpp"


namespace arasy::utils {
    template<typename... Args>
    constexpr std::optional<arasy::error::PushFmtErrorCode> checkPushFmt(const std::string_view fmt) {
        auto idx = fmt.find('%');
        if (idx == std::string_view::npos) {
            return std::nullopt;
        }
        idx = idx + 1;
        if (idx >= fmt.size()) {
            return arasy::error::PushFmtErrorCode::INVALID_SPECIFIER;
        }
        if (fmt.at(idx) != '%') {
            return arasy::error::PushFmtErrorCode::TOO_FEW_ARGS;
        } else {
            return std::nullopt;
        }
    }

    template<typename T1, typename... Args>
    constexpr std::optional<arasy::error::PushFmtErrorCode> checkPushFmt(const std::string_view fmt, T1& firstArg, Args&... args) {
        auto idx = fmt.find('%');
        if (idx == std::string_view::npos) {
            return arasy::error::PushFmtErrorCode::TOO_MANY_ARGS;
        }

        idx++;
        if (idx >= fmt.size()) {
            return arasy::error::PushFmtErrorCode::INVALID_SPECIFIER;
        }

        switch (fmt.at(idx)) {
            case '%':
                return checkPushFmt(fmt.substr(idx+1), firstArg, args...);

            case 'p':
                if constexpr (!std::is_convertible_v<T1, void *>) {
                    return arasy::error::PushFmtErrorCode::INCOMPATIBLE_ARG;
                }
                break;

            case 'd':
            case 'c':
                if constexpr (!std::is_convertible_v<T1, LuaInteger>) {
                    return arasy::error::PushFmtErrorCode::INCOMPATIBLE_ARG;
                }
                break;

            case 'f':
                if constexpr (!std::is_convertible_v<T1, LuaNumber>) {
                    return arasy::error::PushFmtErrorCode::INCOMPATIBLE_ARG;
                }
                break;

            case 's':
                if constexpr (!std::is_convertible_v<T1, LuaString>) {
                    return arasy::error::PushFmtErrorCode::INCOMPATIBLE_ARG;
                }
                break;

            default:
                return arasy::error::PushFmtErrorCode::INVALID_SPECIFIER;
        }

        return checkPushFmt(fmt.substr(idx+1), args...);
    }
}
