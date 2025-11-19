#pragma once

#include "lua.hpp"

#include <ostream>
#include <optional>

namespace arasy::error {
    constexpr const std::nullopt_t none = std::nullopt;

    template<typename E, typename = std::enable_if_t<std::is_enum_v<E>>>
    struct Error {
        E code;
        std::optional<std::string> message = std::nullopt;

        Error(E code_): code(code_) {}
        Error(E code_, const std::string& message_): code(code_), message(message_) {}
    };

    template<typename E, typename = std::enable_if_t<std::is_enum_v<E>>>
    struct MaybeError : std::optional<Error<E>> {
        MaybeError(E code_): optional(Error<E>{code_}) {}
        MaybeError(E code_, const std::string& message_): optional(Error<E>{code_, message_}) {}
        MaybeError(const Error<E>& e): optional(e) {}
        MaybeError(const std::nullopt_t& n): optional(n) {}
        MaybeError(const std::optional<Error<E>>& opt): optional(opt) {}

        bool matches(const E& e) const {
            return has_value() && value().code == e;
        }
    };

    template<typename E, typename = std::enable_if_t<std::is_enum_v<E>>>
    std::ostream& operator<<(std::ostream& os, const Error<E>& err) {
        os << err.code;
        if (err.message.has_value()) {
            os << " (" << *err.message << ")";
        }
        return os;
    }

    template<typename E, typename = std::enable_if_t<std::is_enum_v<E>>>
    std::ostream& operator<<(std::ostream& os, const MaybeError<E>& merr) {
        if (merr.has_value()) {
            return os << *merr;
        } else {
            return os << "<No error>";
        }
    }

    enum class PushFmtErrorCode {
        TOO_FEW_ARGS,
        TOO_MANY_ARGS,
        INVALID_SPECIFIER,
        INCOMPATIBLE_ARG
    };
    using PushFmtError = Error<PushFmtErrorCode>;
    using MPushFmtError = MaybeError<PushFmtErrorCode>;
    std::ostream& operator<<(std::ostream& os, const PushFmtErrorCode& err);

    enum class ScriptErrorCode {
        RUNTIME_ERROR = LUA_ERRRUN,
        SYNTAX_ERROR = LUA_ERRSYNTAX,
        MEMORY_ERROR = LUA_ERRMEM,
        ERROR_DURING_ERROR = LUA_ERRERR,
        IO_ERROR = LUA_ERRFILE,
        UNKNOWN = 100
    };
    using ScriptError = Error<ScriptErrorCode>;
    using MScriptError = MaybeError<ScriptErrorCode>;
    std::ostream& operator<<(std::ostream& os, const ScriptErrorCode& err);

    enum class IndexingErrorCode {
        // If not enough values were present on the stack for the operation
        NOT_ENOUGH_VALUES,

        // If the given key was nil
        NIL_KEY
    };
    using IndexingError = Error<IndexingErrorCode>;
    using MIndexingError = MaybeError<IndexingErrorCode>;
    std::ostream& operator<<(std::ostream& os, const IndexingErrorCode& err);

    // enum class ThreadErrorCode {
    //     UNSPECIFIED
    // };
    // using ThreadError = Error<ThreadErrorCode>;
    // using MThreadError = MaybeError<ThreadErrorCode>;
    // std::ostream& operator<<(std::ostream& os, const ThreadErrorCode& err);
}

namespace arasy {
    constexpr const auto no_error = error::none;
}
