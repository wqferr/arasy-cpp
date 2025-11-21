#pragma once

#include <memory>
#include <type_traits>

#include "arasy/errors.hpp"
#include "arasy/types/callable.hpp"
#include "arasy/types/integer.hpp"

namespace arasy::core {
    class LuaValue;
    class LuaTable;
}

namespace arasy::core::internal {
    class LuaIndexable : public LuaCallable {
    public:
        class IndexedValue {
        public:
            LuaValue key() const;

            template<typename T=LuaValue, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
            std::optional<T> get() const {
                return t.get<T>(*key_);
            }

            operator LuaValue() const;

        private:
            LuaIndexable& t;
            const std::shared_ptr<LuaValue> key_;
            std::shared_ptr<LuaValue> makeKey(const LuaValue& k);
            std::shared_ptr<LuaValue> dummyForArrowOp;

        public:
            IndexedValue(LuaIndexable& t_, const LuaValue& k);

            IndexedValue& operator=(const IndexedValue& other) = delete;
            IndexedValue& operator=(IndexedValue&& other) = default;
            IndexedValue(const IndexedValue& other) = delete;
            IndexedValue(IndexedValue&& other) = default;

            template<typename T, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
            IndexedValue& operator=(const T& value) {
                t.set(*key_, value);
                return *this;
            }

            IndexedValue& operator=(const lua_Number& value);
            IndexedValue& operator=(const char* str);

            template<typename T, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
            void set(const T& value) {
                *this = value;
            }

            void set(const lua_Number& value);
            void set(const char* value);

            const LuaValue& operator*() const;
            std::shared_ptr<const LuaValue> operator->() const;

            friend class LuaIndexable;
        };

        LuaIndexable(lua_State* L, int idx): LuaCallable(L, idx) {}

        error::MIndexingError setStackKV();
        error::MIndexingError set(const LuaValue& key, const LuaValue& value);
        error::MIndexingError setField(const char* fieldName, const LuaValue& value);

        error::MIndexingError setRawStackKV();
        error::MIndexingError setRaw(const LuaValue& key, const LuaValue& value);
        error::MIndexingError setRawi(const LuaInteger& i, const LuaValue& value);

        template<typename T = LuaValue, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
        std::optional<T> get(const LuaValue& key) {
            lua_State* L = registry.luaInstance;
            if (auto err = retrieve(key)) {
                return std::nullopt;
            }
            auto ret = arasy::core::internal::LuaStackReader<T>::readAt(L, -1);
            lua_pop(L, 1);
            return ret;
        }

        template<typename T = LuaValue, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
        std::optional<T> get(const LuaValue& key, std::optional<arasy::error::IndexingError>& err) {
            lua_State* L = registry.luaInstance;
            if (err = retrieve(key)) {
                return std::nullopt;
            }
            auto ret = arasy::core::internal::LuaStackReader<T>::readAt(L, -1);
            lua_pop(L, 1);
            return ret;
        }

        template<typename T = LuaValue, typename = std::enable_if_t<is_lua_wrapper_type_v<T>>>
        std::optional<T> getField(const char* fieldName) {
            lua_State* L = registry.luaInstance;
            retrieveField(fieldName);
            auto ret = arasy::core::internal::LuaStackReader<T>::readAt(L, -1);
            lua_pop(L, 1);
            return ret;
        }

        error::MIndexingError retrieve(const LuaValue& key);
        error::MIndexingError retrieveStackK();
        void retrieveField(const char* fieldName);
        error::MIndexingError retrieveRaw(const LuaValue& key);
        error::MIndexingError retrieveRawStackK();

        void setMetatableStack();
        void setMetatable(const LuaTable& metatable);

        IndexedValue operator[](const LuaValue& k);
        IndexedValue operator[](const char* k);
        IndexedValue operator[](const lua_Integer& i);
    };
}
