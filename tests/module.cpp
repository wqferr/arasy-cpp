#include <gtest/gtest.h>

#include "arasy.hpp"
#include "arasy/module.hpp"
#include "arasy/types/boolean.hpp"

using namespace arasy;
using namespace arasy::utils;
using namespace arasy::literals;

namespace {
    int apiFunc(lua_State* ls) {
        Lua L {ls};
        L.ensureStack(1);
        LuaNumber x = luaL_checknumber(L, 1);
        L.push(2*x.value);
        return 1;
    }
}

struct MySubmodule : public Module {
    declfield(nested);

    MySubmodule(lua_State* L_, const char* name_):
        Module(L_, name_)
    {
        nested = 35_li;
    }
};

struct MyModule : public Module {
    declfield(member1);
    declfield(member2);
    declfield(func);
    declsubmod(sub, MySubmodule);

    MyModule(lua_State* L_, const char* name_):
        Module(L_, name_)
    {
        Lua L {L_};
        member1 = core::LuaBoolean{false};
        member2 = "a string!"_ls;

        func = L.createCFunction(&apiFunc);
    }
};

namespace {
    int failAtPanic(lua_State* L) {
        throw std::runtime_error("Lua panicked");
    }
}


TEST(Module, CanBeLoadedAndRead) {
    Lua L;
    lua_atpanic(L, &failAtPanic);
    luaL_openlibs(L);
    ASSERT_FALSE(L.loadModule<MyModule>("my_mod").has_value());
    L.executeFile("tests/scripts/cmodule.lua");
}
