#pragma once

#include <ostream>

namespace arasy::error {
    enum class PushFmtError {
        NONE,
        TOO_FEW_ARGS,
        TOO_MANY_ARGS,
        INVALID_SPECIFIER,
        INCOMPATIBLE_ARG,
        UNSPECIFIED
    };
    std::ostream& operator<<(std::ostream& os, const PushFmtError& err);

    enum class ExecuteError {
        NONE,
        IO_ERROR,
        LOAD_ERROR,
        RUNTIME_ERROR,
        UNSPECIFIED
    };
    std::ostream& operator<<(std::ostream& os, const PushFmtError& err);
}
