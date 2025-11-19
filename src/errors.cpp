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
                return os << "<I/O error>";
            case ScriptErrorCode::SYNTAX_ERROR:
                return os << "<Syntax error>";
            case ScriptErrorCode::RUNTIME_ERROR:
                return os << "<Runtime error>";
            default:
                return os << unknownErrorMsg;
        }
    }

    int ScriptError::forward(lua_State* L) const {
        if (message.has_value()) {
            lua_pushstring(L, message->c_str());
        } else {
            lua_pushnil(L);
        }
        return static_cast<int>(code);
    }

    std::ostream& operator<<(std::ostream& os, const IndexingErrorCode& err) {
        switch (err) {
            case IndexingErrorCode::NIL_KEY:
                return os << "<Nil key>";
            case IndexingErrorCode::NOT_ENOUGH_VALUES:
                return os << "<Not enough values on the stack>";
            default:
                return os << unknownErrorMsg;
        }
    }

    // std::ostream& operator<<(std::ostream& os, const ThreadErrorCode& err) {
    //     switch (err) {
    //         case ThreadErrorCode::UNSPECIFIED:
    //             return os << "<Unspecified>";
    //         default:
    //             return os << unknownErrorMsg;
    //     }
    // }
}
