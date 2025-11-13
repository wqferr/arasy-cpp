#include "arasy.hpp"

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
            case PushFmtErrorCode::UNSPECIFIED:
                return os << "<Unspecified error>";
            default:
                return os << "<Unknown error code: please contact an Arasy developer>";
        }
    }

    std::ostream& operator<<(std::ostream& os, const ExecuteError& err) {
        switch (err.code) {
            case ExecuteErrorCode::IO_ERROR:
                os << "<I/O error";
            case ExecuteErrorCode::LOAD_ERROR:
                os << "<Load error";
            case ExecuteErrorCode::RUNTIME_ERROR:
                os << "<Runtime error";
            case ExecuteErrorCode::UNSPECIFIED:
                os << "<Unspecified error";
            default:
                return os << "<Unknown error code: please contact an Arasy developer>";
        }
        return os << ": " << err.message << ">";
    }
}
