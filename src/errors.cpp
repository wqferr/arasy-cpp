#include "arasy.hpp"

namespace {
    const char* unknownErrorMsg = "<Unknown error code: please contact an Arasy developer>";
}

namespace arasy::error {
std::ostream& operator<<(std::ostream& os, const PushFmtErrorCode& err) {
        switch (err) {
            case PushFmtErrorCode::TOO_FEW_ARGS:
                return os << "<Too few args>";
            case PushFmtErrorCode::TOO_MANY_ARGS:
                return os << "<Too many args>";
            case PushFmtErrorCode::INVALID_SPECIFIER:
                return os << "<Invalid placeholder>";
            case PushFmtErrorCode::INCOMPATIBLE_ARG:
                return os << "<Incompatible arg>";
            default:
                return os << unknownErrorMsg;
        }
    }

    std::ostream& operator<<(std::ostream& os, const ScriptErrorCode& err) {
        switch (err) {
            case ScriptErrorCode::IO_ERROR:
                os << "<I/O error>";
            case ScriptErrorCode::LOAD_ERROR:
                os << "<Load error>";
            case ScriptErrorCode::RUNTIME_ERROR:
                os << "<Runtime error>";
            default:
                return os << unknownErrorMsg;
        }
    }

    std::ostream& operator<<(std::ostream& os, const TableIndexingErrorCode& err) {
        switch (err) {
            case TableIndexingErrorCode::NIL_KEY:
                os << "<Nil key>";
            case TableIndexingErrorCode::NOT_ENOUGH_VALUES:
                os << "<Not enough values on the stack>";
            default:
                return os << unknownErrorMsg;
        }
    }
}
