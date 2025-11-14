#pragma once

#include <ostream>
#include <optional>

namespace arasy::error {
    constexpr const std::nullopt_t none = std::nullopt;

    template<typename E, typename = std::enable_if_t<std::is_enum_v<E>>>
    struct Error {
        E code;
        std::optional<std::string> message;
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

    enum class TableIndexingErrorCode {
        // If not enough values were present on the stack for the operation
        NOT_ENOUGH_VALUES,

        // If the given key was nil
        NIL_KEY
    };
    using TableIndexingError = Error<TableIndexingErrorCode>;
    std::ostream& operator<<(std::ostream& os, const TableIndexingErrorCode& err);
}

namespace arasy {
    constexpr const auto no_error = error::none;
}
