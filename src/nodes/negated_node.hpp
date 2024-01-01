#ifndef SRC_NODES_NEGATED_NODE_HPP
#define SRC_NODES_NEGATED_NODE_HPP

#include <cstdint>

namespace e_regex {
    template<typename matcher>
    struct negated_node
    {
            static constexpr std::size_t groups = matcher::groups;

            static constexpr auto match(auto result)
            {
                result = matcher::match(std::move(result));

                result = !static_cast<bool>(result);

                return result;
            }
    };
}

#endif /* SRC_NODES_NEGATED_NODE_HPP */
