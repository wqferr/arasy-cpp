#pragma once

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
    std::ostream& operator<<(std::ostream& os, const Error<E>& err) {
        os << err.code;
        if (err.message.has_value()) {
            os << " (" << *err.message << ")";
        }
        return os;
    }

    enum class PushFmtErrorCode {
        TOO_FEW_ARGS,
        TOO_MANY_ARGS,
        INVALID_SPECIFIER,
        INCOMPATIBLE_ARG
    };
    using PushFmtError = Error<PushFmtErrorCode>;
    std::ostream& operator<<(std::ostream& os, const PushFmtErrorCode& err);

    enum class ScriptErrorCode {
        IO_ERROR,
        LOAD_ERROR,
        RUNTIME_ERROR,
        MEMORY_ERROR
    };
    using ScriptError = Error<ScriptErrorCode>;
    std::ostream& operator<<(std::ostream& os, const ScriptErrorCode& err);

    enum class IndexingErrorCode {
        // If not enough values were present on the stack for the operation
        NOT_ENOUGH_VALUES,

        // If the given key was nil
        NIL_KEY
    };
    using IndexingError = Error<IndexingErrorCode>;
    std::ostream& operator<<(std::ostream& os, const IndexingErrorCode& err);

    enum class ThreadErrorCode {
        UNSPECIFIED
    };
    using ThreadError = Error<ThreadErrorCode>;
    std::ostream& operator<<(std::ostream& os, const ThreadErrorCode& err);
}

namespace arasy {
    constexpr const auto no_error = error::none;
}
