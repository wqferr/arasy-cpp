#include "arasy.hpp"
#include "lua.hpp"

using namespace arasy;
using namespace arasy::core;
using namespace arasy::error;

Lua::Lua(): state(luaL_newstate()), registry(state) {}
Lua::Lua(lua_State* L): state(L), external(true), registry(state) {}

Lua::~Lua() {
    if (!external) {
        lua_close(state);
    }
}

void Lua::ensureStack(int n) {
    lua_checkstack(state, n);
}

std::vector<LuaValue> Lua::multiPop(int n) {
    std::vector<LuaValue> values;
    n = std::min(n, stackSize());
    values.reserve(n);
    for (int idx = -n; idx <= -1; idx++) {
        values.push_back(*readStack(idx));
    }
    lua_pop(state, n);
    return values;
}

int Lua::stackSize() const {
    return lua_gettop(state);
}

LuaTable Lua::getGlobalsTable() {
    retrieveGlobalsTable();
    return *popStack<LuaTable>();
}

void Lua::retrieveGlobalsTable() {
    lua_pushglobaltable(state);
}

void Lua::push(const LuaValue& value) {
    value.pushOnto(state);
}

void Lua::retrieveGlobal(const std::string& name) {
    lua_getglobal(state, name.c_str());
}

error::MScriptError Lua::pcall(int narg, int nret, lua_KContext ctx) {
    int runError = lua_pcallk(state, narg, nret, 0, ctx, nullptr);
    if (runError == LUA_ERRRUN) {
        std::string errMsg;
        if (auto luaStr = popStack<LuaString>()) {
            errMsg = luaStr->str();
        } else {
            errMsg = "No error message";
        }
        return MScriptError{
            ScriptErrorCode::RUNTIME_ERROR,
            std::move(errMsg)
        };
    } else if (runError == LUA_ERRMEM) {
        return MScriptError{
            ScriptErrorCode::MEMORY_ERROR,
            "Runtime memory allocation error"
        };
    } else if (runError == LUA_ERRERR) {
        return MScriptError{
            ScriptErrorCode::RUNTIME_ERROR,
            "Memory allocation error"
        };
    } else if (runError != LUA_OK) {
        return MScriptError{
            ScriptErrorCode::RUNTIME_ERROR,
            "Unknown runtime error"
        };
    }

    return std::nullopt;
}

std::optional<LuaValueVarIndex> Lua::type(int idx) const {
    (void)idx;
    std::optional<LuaValue> value = readStackTop();
    if (!value) {
        return std::nullopt;
    }

    return value->luaTypeId();
}

namespace {
    std::optional<ScriptError> checkLoadChunk(int loadError) {
        if (loadError == LUA_ERRSYNTAX) {
            return ScriptError{
                ScriptErrorCode::LOAD_ERROR,
                "Syntax error"
            };
        } else if (loadError == LUA_ERRMEM) {
            return ScriptError{
                ScriptErrorCode::MEMORY_ERROR,
                "Memory allocation error"
            };
        } else if (loadError == LUA_ERRFILE) {
            return ScriptError{
                ScriptErrorCode::IO_ERROR,
                "File I/O error"
            };
        } else if (loadError != LUA_OK) {
            return ScriptError{
                ScriptErrorCode::LOAD_ERROR,
                "Unknown load error"
            };
        } else {
            return std::nullopt;
        }
    }
}


error::MScriptError Lua::loadString(const std::string& code) {
    int loadError = luaL_loadstring(state, code.c_str());
    if (auto err = checkLoadChunk(loadError)) {
        return err;
    } else {
        return std::nullopt;
    }
}

error::MScriptError Lua::loadFile(const std::string& fileName) {
    int loadError = luaL_loadfile(state, fileName.c_str());
    if (auto err = checkLoadChunk(loadError)) {
        return err;
    } else {
        return std::nullopt;
    }
}

error::MScriptError Lua::executeString(const std::string& code) {
    if (auto err = loadString(code.c_str())) {
        return err;
    } else {
        return pcall();
    }
}

error::MScriptError Lua::executeFile(const std::string& fileName) {
    if (auto err = loadFile(fileName.c_str())) {
        return err;
    } else {
        return pcall();
    }
}


// GlobalVarProxy

Lua::GlobalVariableProxy Lua::operator[](const std::string& name) {
    return {*this, name};
}

Lua::GlobalVariableProxy::operator LuaValue() const {
    return value();
}

LuaValue Lua::GlobalVariableProxy::value() const {
    lua_getglobal(L, globalName.c_str());
    return L.popStack().value_or(nil);
}

Lua::GlobalVariableProxy& Lua::GlobalVariableProxy::operator=(const lua_Number& value) {
    return *this = LuaNumber{value};
}

Lua::GlobalVariableProxy& Lua::GlobalVariableProxy::operator=(const char *str) {
    return *this = LuaString{str};
}

void Lua::receive(LuaValue copyOfAlien) {
    copyOfAlien.visit(
        [this](internal::LuaBaseType& value) {
            value.transportTo(this->state);
        }
    );
    push(copyOfAlien);
}

void Lua::pushNewTable() {
    lua_newtable(state);
}

LuaTable Lua::createNewTable() {
    pushNewTable();
    return *popStack<LuaTable>();
}

void Lua::pushCFunction(lua_CFunction cf) {
    lua_pushcfunction(state, cf);
}

LuaCFunction Lua::createCFunction(lua_CFunction cf) {
    pushCFunction(cf);
    return *popStack<LuaCFunction>();
}

void Lua::pushCClosure(lua_CFunction cf, int nUpvalues) {
    lua_pushcclosure(state, cf, nUpvalues);
}

LuaCFunction Lua::createCClosureStackUpvalues(lua_CFunction cf, int nUpvalues) {
    pushCClosure(cf, nUpvalues);
    return *popStack<LuaCFunction>();
}

void Lua::pushInt(lua_Integer i) {
    push(LuaInteger{i});
}

void Lua::pushNum(lua_Number x) {
    push(LuaNumber{x});
}

void Lua::pushStr(const std::string& str) {
    push(LuaString{str.c_str()});
}


std::optional<LuaTable> Lua::makeTable(const std::vector<std::pair<LuaValue, LuaValue>>& entries) {
    lua_newtable(state);

    for (const auto& entry : entries) {
        if (entry.first.isNil()) {
            lua_pop(state, 1);
            return std::nullopt;
        }
        push(entry.first);
        push(entry.second);
        lua_settable(state, -3);
    }

    return *popStack<LuaTable>();
}
