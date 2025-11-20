#include "arasy/types/thread.hpp"
#include "arasy.hpp"
#include <iomanip>

using namespace arasy::core;

LuaThread::LuaThread(lua_State* L): thread_(std::make_shared<Lua>(L)) {}

void LuaThread::pushOnto(lua_State* L) const {
    lua_pushthread(thread_->state);
    lua_xmove(thread_->state, L, 1);
}

bool arasy::core::operator==(const LuaThread& a, const LuaThread& b) {
    return a.lua().state == b.lua().state;
}

namespace arasy::core::thread {
    std::ostream& operator<<(std::ostream& os, const ResumeResult& r) {
        if (r.isOk()) {
            os << "Ok(finished=" << r->finished << ", nret=" << r->nret << ")";
        } else {
            auto& err = r.error();
            os << "ScriptError(code=" << err.code << ", message=" << std::quoted(
                err.message.has_value() ? *err.message : std::string{"<No message>"}
            ) << ")";
        }
        return os;
    }
}
