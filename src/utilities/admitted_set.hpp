#ifndef UTILITIES_ADMITTED_SET_HPP
#define UTILITIES_ADMITTED_SET_HPP

#include <array>
#include <limits>
#include <utility>

namespace e_regex
{
    template<typename Char, Char... chars_>
    struct admitted_set
    {
            using Char_t                                               = Char;
            static constexpr auto                                empty = sizeof...(chars_) == 0;
            static constexpr std::array<Char, sizeof...(chars_)> chars {chars_...};
    };

    // Generate an admitted_set from a range
    template<typename Char, Char start, Char end, typename seq = std::make_integer_sequence<std::size_t, 1 + end - start>>
        requires(end > start)
    struct admitted_set_range;

    template<typename Char, Char start, Char end, auto... seq>
    struct admitted_set_range<Char, start, end, std::integer_sequence<std::size_t, seq...>>
    {
            using type = admitted_set<Char, (static_cast<Char>(seq) + start)...>;
    };

    template<typename Char, Char start, Char end>
    using admitted_set_range_t = typename admitted_set_range<Char, start, end>::type;

    // Concatenate two admitted sets
    template<typename set1, typename set2, typename difference = admitted_set<typename set1::Char_t>>
    struct merge_admitted_sets;

    template<typename Char, Char common, Char... chars1, Char... chars2, Char... current>
    struct merge_admitted_sets<admitted_set<Char, common, chars1...>,
                               admitted_set<Char, common, chars2...>,
                               admitted_set<Char, current...>>
    {
            // Common char found
            using type = typename merge_admitted_sets<admitted_set<Char, chars1...>,
                                                      admitted_set<Char, chars2...>,
                                                      admitted_set<Char, current..., common>>::type;
    };

    template<typename Char, Char... chars2, Char... current>
    struct merge_admitted_sets<admitted_set<Char>, admitted_set<Char, chars2...>, admitted_set<Char, current...>>
    {
            // First set is empty, finished
            using type = admitted_set<Char, current..., chars2...>;
    };

    template<typename Char, Char... chars1, Char... current>
    struct merge_admitted_sets<admitted_set<Char, chars1...>, admitted_set<Char>, admitted_set<Char, current...>>
    {
            // Second set is empty, finished
            using type = admitted_set<Char, current..., chars1...>;
    };

    template<typename Char, Char... current>
    struct merge_admitted_sets<admitted_set<Char>, admitted_set<Char>, admitted_set<Char, current...>>
    {
            // Both sets are empty, finished
            using type = admitted_set<Char, current...>;
    };

    template<typename Char, Char head1, Char head2, Char... chars1, Char... chars2, Char... current>
        requires(head1 < head2)
    struct merge_admitted_sets<admitted_set<Char, head1, chars1...>,
                               admitted_set<Char, head2, chars2...>,
                               admitted_set<Char, current...>>
    {
            using type = typename merge_admitted_sets<admitted_set<Char, chars1...>,
                                                      admitted_set<Char, head2, chars2...>,
                                                      admitted_set<Char, current..., head1>>::type;
    };

    template<typename Char, Char head1, Char head2, Char... chars1, Char... chars2, Char... current>
        requires(head1 > head2)
    struct merge_admitted_sets<admitted_set<Char, head1, chars1...>,
                               admitted_set<Char, head2, chars2...>,
                               admitted_set<Char, current...>>
    {
            // head2 is in difference
            using type = typename merge_admitted_sets<admitted_set<Char, head1, chars1...>,
                                                      admitted_set<Char, chars2...>,
                                                      admitted_set<Char, current..., head2>>::type;
    };

    template<typename set1, typename set2>
    using merge_admitted_sets_t = typename merge_admitted_sets<set1, set2>::type;

    // Difference of two sets
    template<typename set1, typename set2, typename difference = admitted_set<typename set1::Char_t>>
    struct admitted_sets_difference;

    template<typename Char, Char common, Char... chars1, Char... chars2, Char... current>
    struct admitted_sets_difference<admitted_set<Char, common, chars1...>,
                                    admitted_set<Char, common, chars2...>,
                                    admitted_set<Char, current...>>
    {
            // Common char found, discard it
            using type = typename admitted_sets_difference<admitted_set<Char, chars1...>,
                                                           admitted_set<Char, chars2...>,
                                                           admitted_set<Char, current...>>::type;
    };

    template<typename Char, Char... chars2, Char... current>
    struct admitted_sets_difference<admitted_set<Char>, admitted_set<Char, chars2...>, admitted_set<Char, current...>>
    {
            // First set is empty, finished
            using type = admitted_set<Char, current..., chars2...>;
    };

    template<typename Char, Char... chars1, Char... current>
    struct admitted_sets_difference<admitted_set<Char, chars1...>, admitted_set<Char>, admitted_set<Char, current...>>
    {
            // Second set is empty, finished
            using type = admitted_set<Char, current..., chars1...>;
    };

    template<typename Char, Char... current>
    struct admitted_sets_difference<admitted_set<Char>, admitted_set<Char>, admitted_set<Char, current...>>
    {
            // Both sets are empty, finished
            using type = admitted_set<Char, current...>;
    };

    template<typename Char, Char head1, Char head2, Char... chars1, Char... chars2, Char... current>
        requires(head1 < head2)
    struct admitted_sets_difference<admitted_set<Char, head1, chars1...>,
                                    admitted_set<Char, head2, chars2...>,
                                    admitted_set<Char, current...>>
    {
            // head1 is in difference
            using type = typename admitted_sets_difference<admitted_set<Char, chars1...>,
                                                           admitted_set<Char, head2, chars2...>,
                                                           admitted_set<Char, current..., head1>>::type;
    };

    template<typename Char, Char head1, Char head2, Char... chars1, Char... chars2, Char... current>
        requires(head1 > head2)
    struct admitted_sets_difference<admitted_set<Char, head1, chars1...>,
                                    admitted_set<Char, head2, chars2...>,
                                    admitted_set<Char, current...>>
    {
            // head2 is in difference
            using type = typename admitted_sets_difference<admitted_set<Char, head1, chars1...>,
                                                           admitted_set<Char, chars2...>,
                                                           admitted_set<Char, current..., head2>>::type;
    };

    template<typename set1, typename set2>
    using admitted_sets_difference_t = typename admitted_sets_difference<set1, set2>::type;

    // Admitted set complement
    template<typename set, typename Char = typename set::Char_t>
    using admitted_set_complement_t
        = admitted_sets_difference_t<set, admitted_set_range_t<Char, 0, std::numeric_limits<Char>::max()>>;

    // Intersection of two sets
    template<typename set1, typename set2, typename intersection = admitted_set<typename set1::Char_t>>
    struct admitted_sets_intersection;

    template<typename Char, Char common, Char... chars1, Char... chars2, Char... current>
    struct admitted_sets_intersection<admitted_set<Char, common, chars1...>,
                                      admitted_set<Char, common, chars2...>,
                                      admitted_set<Char, current...>>
    {
            // Common char found
            using type =
                typename admitted_sets_intersection<admitted_set<Char, chars1...>,
                                                    admitted_set<Char, chars2...>,
                                                    admitted_set<Char, current..., common>>::type;
    };

    template<typename Char, Char... chars2, Char... current>
    struct admitted_sets_intersection<admitted_set<Char>, admitted_set<Char, chars2...>, admitted_set<Char, current...>>
    {
            // First set is empty, finished
            using type = admitted_set<Char, current...>;
    };

    template<typename Char, Char... chars1, Char... current>
    struct admitted_sets_intersection<admitted_set<Char, chars1...>, admitted_set<Char>, admitted_set<Char, current...>>
    {
            // Second set is empty, finished
            using type = admitted_set<Char, current...>;
    };

    template<typename Char, Char... current>
    struct admitted_sets_intersection<admitted_set<Char>, admitted_set<Char>, admitted_set<Char, current...>>
    {
            // Both sets are empty, finished
            using type = admitted_set<Char, current...>;
    };

    template<typename Char, Char head1, Char head2, Char... chars1, Char... chars2, Char... current>
        requires(head1 < head2)
    struct admitted_sets_intersection<admitted_set<Char, head1, chars1...>,
                                      admitted_set<Char, head2, chars2...>,
                                      admitted_set<Char, current...>>
    {
            // head1 is not common
            using type = typename admitted_sets_intersection<admitted_set<Char, chars1...>,
                                                             admitted_set<Char, head2, chars2...>,
                                                             admitted_set<Char, current...>>::type;
    };

    template<typename Char, Char head1, Char head2, Char... chars1, Char... chars2, Char... current>
        requires(head1 > head2)
    struct admitted_sets_intersection<admitted_set<Char, head1, chars1...>,
                                      admitted_set<Char, head2, chars2...>,
                                      admitted_set<Char, current...>>
    {
            // head2 is not common
            using type = typename admitted_sets_intersection<admitted_set<Char, head1, chars1...>,
                                                             admitted_set<Char, chars2...>,
                                                             admitted_set<Char, current...>>::type;
    };

    template<typename set1, typename set2>
    using admitted_sets_intersection_t = typename admitted_sets_intersection<set1, set2>::type;
}// namespace e_regex

#endif /* UTILITIES_ADMITTED_SET_HPP */
