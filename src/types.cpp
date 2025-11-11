#include "types/all.hpp"

#include "util.hpp"

#include <variant>
#include <iomanip>

using namespace arasy::core;
using namespace arasy::core::internal;

namespace arasy::core {
    std::ostream& operator<<(std::ostream& os, const LuaValue& lv) {
        const auto visitor = internal::overloads{
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
                os << '"' << std::quoted(s.str) << '"';
            },
            [&os](const LuaNil&) {
                os << "nil";
            },
            [&os](const LuaCFunction& cf) {
                os << "<cfunction @ " << std::hex << cf.cfunc << ">";
            }
        };
        std::visit(visitor, lv);
        return os;
    }
}
