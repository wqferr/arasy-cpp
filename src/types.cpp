#include "types/all.hpp"

#include <variant>
#include <iomanip>

using namespace arasy::core;
using namespace arasy::core::internal;

namespace arasy::core {
    std::ostream& operator<<(std::ostream& os, const LuaValue& lv) {
        struct visitor {
            std::ostream& os;
            visitor(std::ostream& os_): os(os_) {}

            void operator()(const LuaBoolean& b) {
                os << b.value;
            }

            void operator()(const LuaInteger& i) {
                os << i.value;
            }

            void operator()(const LuaNumber& x) {
                os << x.value;
            }

            void operator()(const LuaString& s) {
                os << '"' << std::quoted(s.str) << '"';
            }
            void operator()(const LuaNil&) {
                os << "nil";
            }

            void operator()(const LuaCFunction& cf) {
                os << "<function: " << std::hex << cf.cfunc << ">";
            }
        };

        std::visit(visitor{os}, lv);
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
