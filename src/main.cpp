#include "lua.hpp"

int main(void) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    luaL_dostring(L, "x = 1 + #({1, 2, 3}); print(x == 4)");

    lua_close(L);
    return 0;
}
