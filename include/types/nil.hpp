#pragma once

namespace arasy::core {
    struct LuaNil {
        constexpr bool operator==(const LuaNil& other) const { return true; }
    };
}

namespace arasy {
    constexpr const inline core::LuaNil nil {};
}
