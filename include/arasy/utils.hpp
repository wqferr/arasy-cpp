#pragma once

namespace arasy::utils {
    namespace internal {
        template<typename... Ts> struct overload : Ts... { using Ts::operator()...; };
        template<typename... Ts> overload(Ts...) -> overload<Ts...>;
    }
}
