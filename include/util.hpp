#pragma once

namespace arasy::core::internal {
    template<typename... Ts>
    struct overloads : Ts... {
        using Ts::operator()...;
    };
}
