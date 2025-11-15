#include "arasy/types/all.hpp"
#include "arasy/utils.hpp"
#include "arasy.hpp"

#include <variant>
#include <iomanip>

using namespace arasy::core;
using namespace arasy::core::internal;

namespace arasy::core {

    std::optional<lua_Number> LuaValue::toNumber() const {
        if (isA<LuaInteger>()) {
            return asA<LuaInteger>().value;
        } else if (isA<LuaNumber>()) {
            return asA<LuaNumber>().value;
        } else {
            return std::nullopt;
        }
    }

    bool LuaValue::isNumeric() const {
        return isA<LuaInteger>() || isA<LuaNumber>();
    }

    std::ostream& operator<<(std::ostream& os, const LuaValue& lv) {
        struct visitor {
            std::ostream& os;
        };

        std::visit(
            utils::internal::overload{
                [&os](const LuaBoolean& b) {
                    os << b.value;
                },
                [&os](const LuaInteger& i) {
                    os << i.value;
                },
                [&os](const LuaNumber& x) {
                    os << x.value;
                },
                [&os](const LuaString& s) {
                    os << '"' << std::quoted(s.str()) << '"';
                },
                [&os](const LuaNil&) {
                    os << "nil";
                },
                [&os](const LuaCFunction& cf) {
                    os << "function: " << std::hex << cf.cfunc;
                },
                [&os](const LuaThread& thr) {
                    os << "thread: " << std::hex << thr.thread().state;
                },
                [&os](const LuaTable& tbl) {
                    os << "table: id #" << tbl.id();
                }
            },
            lv
        );
        return os;
    }

    namespace internal {
        std::optional<LuaValue> LuaStackReader<LuaValue>::readAt(lua_State* L, int idx) {
            if (LuaStackReader<LuaNil>::checkAt(L, idx)) {
                return nil;
            } else if (LuaStackReader<LuaInteger>::checkAt(L, idx)) {
                return LuaStackReader<LuaInteger>::readAt(L, idx);
            } else if (LuaStackReader<LuaNumber>::checkAt(L, idx)) {
                return LuaStackReader<LuaNumber>::readAt(L, idx);
            } else if (LuaStackReader<LuaString>::checkAt(L, idx)) {
                return LuaStackReader<LuaString>::readAt(L, idx);
            } else if (LuaStackReader<LuaTable>::checkAt(L, idx)) {
                return LuaStackReader<LuaTable>::readAt(L, idx);
            } else if (LuaStackReader<LuaThread>::checkAt(L, idx)) {
                return LuaStackReader<LuaThread>::readAt(L, idx);
            } else if (LuaStackReader<LuaCFunction>::checkAt(L, idx)) {
                return LuaStackReader<LuaCFunction>::readAt(L, idx);
            } else if (LuaStackReader<LuaBoolean>::checkAt(L, idx)) {
                return LuaStackReader<LuaBoolean>::readAt(L, idx);
            } else {
                return nil;
            }
        }
    }
}
