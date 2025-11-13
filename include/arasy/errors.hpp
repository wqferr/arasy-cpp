#pragma once

#include <ostream>

namespace arasy::error {
    enum class PushFmtErrorCode {
        TOO_FEW_ARGS,
        TOO_MANY_ARGS,
        INVALID_SPECIFIER,
        INCOMPATIBLE_ARG,
        UNSPECIFIED
    };
    std::ostream& operator<<(std::ostream& os, const PushFmtErrorCode& err);

    enum class ExecuteErrorCode {
        IO_ERROR,
        LOAD_ERROR,
        RUNTIME_ERROR,
        UNSPECIFIED
    };
    struct ExecuteError {
        ExecuteErrorCode code;
        std::string message;
    };
    std::ostream& operator<<(std::ostream& os, const ExecuteError& err);
}
