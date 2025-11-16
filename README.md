# Arasy
A Lua scripting enabler library you can embed in your C++ applications and games.

## Name
Arasy is the Tupi-Guarani goddess of the moon.

## Panics
- If a `LuaString` is initialized with a `nullptr`, a `std::runtime_error` is thrown.
- If a context-aware value is pushed onto another Lua instance (e.g., tables, userdata, C functions).
