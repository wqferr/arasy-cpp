#pragma once

#include <type_traits>

namespace arasy::utils {
    namespace internal {
        template<typename... Ts> struct overload : Ts... { using Ts::operator()...; };
        template<typename... Ts> overload(Ts...) -> overload<Ts...>;

        template<typename A, typename... Ts>
        constexpr bool is_any_of_v = (std::is_same_v<A, Ts> || ...);
    }
}
