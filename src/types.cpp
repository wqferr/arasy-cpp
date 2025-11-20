#include "arasy/types.hpp"
#include "arasy/utils.hpp"
#include "arasy.hpp"

#include <variant>
#include <iomanip>

using namespace arasy::core;
using namespace arasy::core::internal;

namespace arasy::core {
    std::optional<lua_Number> LuaValue::toNumber() const {
        if (isA<LuaNumber>()) {
            return asA<LuaNumber>().value;
        } else {
            return std::nullopt;
        }
    }

    std::ostream& operator<<(std::ostream& os, const LuaValueType& value) {
        using V = LuaValueType;
        switch (value) {
            case V::LuaBoolean:
                return os << "<type:boolean>";
            case V::LuaInteger:
                return os << "<type:integer>";
            case V::LuaNumber:
                return os << "<type:number>";
            case V::LuaNil:
                return os << "<type:nil>";
            case V::LuaString:
                return os << "<type:string>";
            case V::LuaTable:
                return os << "<type:table>";
            case V::LuaFunction:
                return os << "<type:function>";
            case V::LuaCFunction:
                return os << "<type:cfunction>";
            case V::LuaThread:
                return os << "<type:thread>";
            case V::LuaLightUserData:
                return os << "<type:lightuserdata>";
            case V::LuaFullUserData:
                return os << "<type:fulluserdata>";
        }
        return os << "<type:INVALID>";
    }


    bool LuaValue::isNumeric() const {
        return isA<LuaNumber>();
    }

    std::ostream& operator<<(std::ostream& os, const LuaValue& lv) {
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
                    os << std::quoted(s.str());
                },
                [&os](const LuaNil&) {
                    os << "nil";
                },
                [&os](const LuaFunction& f) {
                    os << "function: id #" << std::hex << f.id();
                },
                [&os](const LuaCFunction& cf) {
                    os << "cfunction: " << std::hex << cf.cfunc;
                },
                [&os](const LuaThread& thr) {
                    os << "thread: " << std::hex << thr.lua().state;
                },
                [&os](const LuaTable& tbl) {
                    os << "table: id #" << tbl.id();
                },
                [&os](const LuaLightUserData& lud) {
                    os << "lightuserdata: " << std::hex << lud.ptr();
                },
                [&os](const LuaFullUserData& lud) {
                    os << "fulluserdata: id #" << lud.id();
                }
            },
            lv
        );
        return os;
    }

    namespace internal {
        std::optional<LuaValue> LuaStackReader<LuaValue>::readAt(lua_State* L, int idx) {
            if (lua_isnone(L, idx)) {
                return std::nullopt;
            } else if (LuaStackReader<LuaNil>::checkAt(L, idx)) {
                return nil;
            } else if (LuaStackReader<LuaBoolean>::checkAt(L, idx)) {
                return LuaStackReader<LuaBoolean>::readAt(L, idx);
            } else if (LuaStackReader<LuaInteger>::checkAt(L, idx)) {
                return LuaStackReader<LuaInteger>::readAt(L, idx);
            } else if (LuaStackReader<LuaNumber>::checkAt(L, idx)) {
                return LuaStackReader<LuaNumber>::readAt(L, idx);
            } else if (LuaStackReader<LuaThread>::checkAt(L, idx)) {
                return LuaStackReader<LuaThread>::readAt(L, idx);
            } else if (LuaStackReader<LuaLightUserData>::checkAt(L, idx)) {
                return LuaStackReader<LuaLightUserData>::readAt(L, idx);
            } else if (LuaStackReader<LuaFullUserData>::checkAt(L, idx)) {
                return LuaStackReader<LuaFullUserData>::readAt(L, idx);
            } else if (LuaStackReader<LuaString>::checkAt(L, idx)) {
                return LuaStackReader<LuaString>::readAt(L, idx);
            } else if (LuaStackReader<LuaTable>::checkAt(L, idx)) {
                return LuaStackReader<LuaTable>::readAt(L, idx);
            } else if (LuaStackReader<LuaCFunction>::checkAt(L, idx)) {
                return LuaStackReader<LuaCFunction>::readAt(L, idx);
            } else if (LuaStackReader<LuaFunction>::checkAt(L, idx)) {
                return LuaStackReader<LuaFunction>::readAt(L, idx);
            } else {
                throw std::runtime_error("Unknown lua type!");
            }
        }
    }
}
