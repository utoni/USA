#pragma once

#include <string>

namespace Optimization {
    struct TransparentHash {
        using is_transparent = void; // activate heterogeneous lookup

        std::size_t operator()(const std::string& s) const noexcept {
            return std::hash<std::string_view>{}(s);
        }
    };

    struct TransparentEqual {
        using is_transparent = void; // activate heterogenous lookup

        bool operator()(const std::string& lhs,
                        const std::string& rhs) const noexcept
        {
            return lhs == rhs;
        }
    };
}
