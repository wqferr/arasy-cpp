#pragma once

#include <type_traits>
#include <string>
#include "arasy/types.hpp"

#ifndef declfield
#define declfield(ident) Field ident = field(#ident);
#endif

#ifndef declsubmod
#define declsubmod(ident, type) type ident = submodule<type>(#ident);
#endif

// TODO: use package.load to lazy load modules

namespace arasy::utils {
    class Module {
    public:
        struct Field {
            const char* const name;
            Module& parent;

        private:
            mutable core::LuaValue cachedValue_;

        public:
            Field(Module& parent_, const char* name_);

            core::LuaValue value() const;
            core::LuaValue cachedValue() const;

            void set(const core::LuaValue& value);
            Field& operator=(const core::LuaValue& value);

            const core::LuaValue* operator->() const;
        };

        void setRef(int ref);
        int ref() const;
        void unref();

        lua_State* const L;
        const std::string name;

        Module(lua_State* L_, const char* name_);
    private:
        internal::non_owning_ptr<Module> parent;
        core::LuaTable makeMembersTable(lua_State* L_);
        int ref_ = LUA_NOREF;

    protected:
        core::LuaTable members;

        Field field(const char* name);

        template<typename M, typename = std::enable_if_t<std::is_base_of_v<Module, M>>>
        M submodule(const char* name_) {
            std::string fullName = name;
            fullName += ".";
            fullName += name_;
            M mod {L, fullName.c_str()};
            mod.parent = this;
            members.setField(name_, mod.members);
            return mod;
        }
    };
}
