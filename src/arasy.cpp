#include "arasy.hpp"
#include "lua.hpp"

using namespace arasy;
using namespace arasy::core;
using namespace arasy::error;

void Lua::multiPop(int n) {
    lua_pop(state, n);
}

int Lua::stackSize() const {
    return lua_gettop(state);
}

void Lua::push(const LuaValue& value) {
    value.pushOnto(state);
}

LuaValue Lua::getGlobal(const std::string& name) {
    lua_pushstring(state, name.c_str());
    return popStack().value_or(nil);
}

std::optional<ScriptError> Lua::pcall(int narg, int nret, lua_KContext ctx) {
    int runError = lua_pcall(state, narg, nret, ctx);
    if (runError == LUA_ERRRUN) {
        std::string errMsg;
        if (auto luaStr = popStack<LuaString>()) {
            errMsg = luaStr->str();
        } else {
            errMsg = "No error message";
        }
        return ScriptError{
            ScriptErrorCode::RUNTIME_ERROR,
            std::move(errMsg)
        };
    } else if (runError == LUA_ERRMEM) {
        return ScriptError{
            ScriptErrorCode::MEMORY_ERROR,
            "Runtime memory allocation error"
        };
    } else if (runError == LUA_ERRERR) {
        return ScriptError{
            ScriptErrorCode::RUNTIME_ERROR,
            "Memory allocation error"
        };
    } else if (runError != LUA_OK) {
        return ScriptError{
            ScriptErrorCode::RUNTIME_ERROR,
            "Unknown runtime error"
        };
    }

    return std::nullopt;
}

std::optional<LuaValueVarIndex> Lua::type(int idx) const {
    std::optional<LuaValue> value = readStackTop();
    if (!value) {
        return std::nullopt;
    }

    return value->luaTypeId();
}

namespace {
    std::optional<ScriptError> checkLoadChunk(Lua& L, int loadError) {
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


std::optional<ScriptError> Lua::loadString(const std::string& code) {
    int loadError = luaL_loadstring(state, code.c_str());
    if (auto err = checkLoadChunk(*this, loadError)) {
        return err;
    } else {
        return std::nullopt;
    }
}

std::optional<ScriptError> Lua::loadFile(const std::string& fileName) {
    int loadError = luaL_loadfile(state, fileName.c_str());
    if (auto err = checkLoadChunk(*this, loadError)) {
        return err;
    } else {
        return std::nullopt;
    }
}

std::optional<ScriptError> Lua::executeString(const std::string& code) {
    if (auto err = loadString(code.c_str())) {
        return err;
    } else {
        return pcall();
    }
}

std::optional<ScriptError> Lua::executeFile(const std::string& fileName) {
    if (auto err = loadFile(fileName.c_str())) {
        return err;
    } else {
        return pcall();
    }
}


// GlobalVarProxy

Lua::GlobalVariableProxy& Lua::operator[](const std::string& name) {
    latestVariableAccessed.emplace(*this, name);
    return *latestVariableAccessed;
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
