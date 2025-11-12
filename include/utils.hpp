#pragma once

namespace arasy::utils {
    namespace internal {
        template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
        template<class... Ts> overload(Ts...) -> overload<Ts...>;
    }
}
