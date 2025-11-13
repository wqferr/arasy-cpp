#pragma once

#include <ostream>

namespace arasy::error {
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
