#pragma once

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

            void set(const lua_Number& value) {
                *this = value;
            }

            void set(const char* value) {
                *this = value;
            }

            friend class LuaIndexable;
        };

        LuaIndexable(lua_State* L, int idx): LuaCallable(L, idx) {}

        std::optional<arasy::error::IndexingError> setStackKV();
        std::optional<arasy::error::IndexingError> set(const LuaValue& key, const LuaValue& value);
        std::optional<arasy::error::IndexingError> setField(const char* fieldName, const LuaValue& value);

        std::optional<arasy::error::IndexingError> setRawStackKV();
        std::optional<arasy::error::IndexingError> setRaw(const LuaValue& key, const LuaValue& value);
        std::optional<arasy::error::IndexingError> setRawi(const LuaInteger& i, const LuaValue& value);

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

        std::optional<arasy::error::IndexingError> retrieve(const LuaValue& key);
        std::optional<arasy::error::IndexingError> retrieveStackK();
        void retrieveField(const char* fieldName);
        std::optional<arasy::error::IndexingError> retrieveRaw(const LuaValue& key);
        std::optional<arasy::error::IndexingError> retrieveRawStackK();

        void setMetatableStack();
        void setMetatable(const LuaTable& metatable);

        IndexedValue operator[](const LuaValue& k);
        IndexedValue operator[](const char* k);
        IndexedValue operator[](const lua_Integer& i);

        template<typename T, typename = std::enable_if_t<is_nonvariant_lua_wrapper_type_v<T>>>
        IndexedValue operator[](const T& k) {
            return (*this)[LuaValue{k}];
        }

        // TODO
        struct OperationChain {};
        OperationChain chain();
    };

    // template<typename T, typename = std::enable_if_t<is_convertible_to_lua_type_v<T>>>
    // bool operator==(const LuaIndexable::IndexedValue& indexed, const T& other) {
    //     return indexed.value() == other;
    // }

    // template<typename T, typename = std::enable_if_t<is_convertible_to_lua_type_v<T>>>
    // bool operator==(const T& other, const LuaIndexable::IndexedValue& indexed) {
    //     return other == indexed.value();
    // }
}
