#pragma once

#include <ostream>
#include <optional>

namespace arasy::error {
    constexpr const std::nullopt_t none = std::nullopt;

    enum class PushFmtErrorCode {
        TOO_FEW_ARGS,
        TOO_MANY_ARGS,
        INVALID_SPECIFIER,
        INCOMPATIBLE_ARG
    };
    std::ostream& operator<<(std::ostream& os, const PushFmtErrorCode& err);

    enum class ScriptErrorCode {
        IO_ERROR,
        LOAD_ERROR,
        RUNTIME_ERROR,
        MEMORY_ERROR
    };
    struct ScriptError {
        ScriptErrorCode code;
        std::string message;
    };
    std::ostream& operator<<(std::ostream& os, const ScriptError& err);
}

namespace arasy {
    constexpr const auto no_error = error::none;
}
