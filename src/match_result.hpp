#ifndef MATCH_RESULT
#define MATCH_RESULT

#include <array>

#include "utilities/literal_string_view.hpp"

namespace e_regex
{
    template<std::size_t groups, typename Char_Type>
    struct match_result_data
    {
            literal_string_view<Char_Type>                     query;
            typename literal_string_view<Char_Type>::iterator  actual_iterator_start;
            typename literal_string_view<Char_Type>::iterator  actual_iterator_end;
            std::array<literal_string_view<Char_Type>, groups> match_groups = {};
            bool                                               accepted     = true;

            constexpr auto operator=(bool accepted) noexcept -> match_result_data &
            {
                this->accepted = accepted;

                return *this;
            }

            constexpr operator bool() const noexcept
            {
                return accepted;
            }
    };

    template<typename matcher, typename Char_Type = char>
    class match_result
    {
        public:
            using expression = typename matcher::expression;

        private:
            match_result_data<matcher::groups, Char_Type> data;

        public:
            constexpr match_result(literal_string_view<> query) noexcept
            {
                data.query                 = query;
                data.actual_iterator_start = query.begin();
                data.actual_iterator_end   = data.actual_iterator_start;

                data = matcher::match(data);
                if (!data)
                {
                    next();
                }
            }

            constexpr auto operator=(bool accepted) noexcept -> match_result &
            {
                this->accepted = accepted;

                return *this;
            }

            constexpr operator bool() const noexcept
            {
                return data;
            }

            constexpr auto is_accepted() const noexcept
            {
                return operator bool();
            }

            template<std::size_t index>
            constexpr auto get() const noexcept
            {
                static_assert(index <= matcher::groups,
                              "Group index is greater than the number of groups.");

                return get_group(index);
            }

            constexpr auto get_group(std::size_t index) const noexcept
            {
                if (index == 0)
                {
                    return to_view();
                }

                return static_cast<std::string_view>(data.match_groups[index - 1]);
            }

            constexpr auto operator[](std::size_t index) const noexcept
            {
                return get_group(index);
            }

            constexpr auto to_view() const noexcept
            {
                if (!is_accepted())
                {
                    return std::string_view {};
                }

                return std::string_view {data.actual_iterator_start, data.actual_iterator_end};
            }

            constexpr operator literal_string_view<Char_Type>() const noexcept
            {
                return to_view();
            }

            constexpr auto size() const noexcept
            {
                return data.matches;
            }

            static constexpr auto groups() noexcept
            {
                return matcher::groups;
            }

            /**
             * @brief Iterate matches
             *
             * @return false if there are no other matches
             */
            constexpr auto next() noexcept
            {
                data.actual_iterator_start = data.actual_iterator_end;

                while (data.actual_iterator_start < data.query.end())
                {
                    data.match_groups        = {};
                    data.actual_iterator_end = data.actual_iterator_start;
                    data.accepted            = true;
                    auto result              = matcher::match(data);

                    if (result)
                    {
                        data = result;
                        return true;
                    }

                    data.actual_iterator_start++;
                }

                data.accepted = false;
                return false;
            }
    };
}// namespace e_regex

// For structured decomposition
namespace std
{
    template<typename matcher>
    struct tuple_size<e_regex::match_result<matcher>>
    {
            static const std::size_t value = matcher::groups + 1;
    };

    template<std::size_t N, typename matcher, typename Char_Type>
    struct tuple_element<N, e_regex::match_result<matcher, Char_Type>>
    {
            using type = std::string_view;
    };

    template<std::size_t N, typename matcher>
    auto get(e_regex::match_result<matcher> t) noexcept
    {
        return t.template get<N>();
    }

}// namespace std

#endif /* MATCH_RESULT */
