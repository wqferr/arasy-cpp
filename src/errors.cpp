#include "arasy.hpp"

namespace arasy::error {
    std::ostream& operator<<(std::ostream& os, const PushFmtError& err) {
        switch (err) {
            case PushFmtError::NONE:
                return os << "<No error>";
            case PushFmtError::TOO_FEW_ARGS:
                return os << "<Too few args>";
            case PushFmtError::TOO_MANY_ARGS:
                return os << "<Too many args>";
            case PushFmtError::INVALID_SPECIFIER:
                return os << "<Invalid placeholder>";
            case PushFmtError::INCOMPATIBLE_ARG:
                return os << "<Incompatible arg>";
            case PushFmtError::UNSPECIFIED:
                return os << "<Unspecified error>";
        }
        return os;
    }
}
