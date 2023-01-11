#ifndef MATCH_RESULT
#define MATCH_RESULT

#include <array>
#include <string_view>

namespace e_regex
{
    template<std::size_t groups>
    struct match_result_data
    {
            std::string_view                     query;
            std::string_view::const_iterator     actual_iterator_start;
            std::string_view::const_iterator     actual_iterator_end;
            std::array<std::string_view, groups> match_groups;
            std::size_t                          matches  = 0;
            bool                                 accepted = true;

            constexpr auto operator=(bool accepted) noexcept -> match_result_data&
            {
                this->accepted = accepted;

                return *this;
            }

            constexpr operator bool() const noexcept
            {
                return accepted;
            }
    };

    template<typename matcher>
    class match_result
    {
            friend matcher;

        private:
            match_result_data<matcher::groups> data;
            bool                               initialized = false;

            constexpr void init()
            {
                initialized              = true;
                data.matches             = 0;
                data.match_groups        = {};
                data.actual_iterator_end = data.actual_iterator_start;
                data.accepted            = true;

                data = matcher::match(std::move(data));
            }

        public:
            constexpr match_result(std::string_view query) noexcept
            {
                data.query                 = query;
                data.actual_iterator_start = query.begin();

                this->next();
            }

            constexpr auto operator=(bool accepted) noexcept -> match_result&
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

                return data.match_groups[index - 1];
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

            constexpr operator std::string_view() const noexcept
            {
                return to_view();
            }

            constexpr auto size() const noexcept
            {
                return data.matches;
            }

            /**
             * @brief Iterate matches
             *
             * @return false if there are no other matches
             */
            constexpr auto next() noexcept
            {
                do
                {
                    if (initialized)
                    {
                        // This is not the first call
                        if (!data.accepted)
                        {
                            data.actual_iterator_start++;
                        }
                        else
                        {
                            data.actual_iterator_start = data.actual_iterator_end;
                        }
                    }

                    init();
                } while (!data.accepted && data.actual_iterator_start < data.query.end()
                         && data.actual_iterator_start >= data.query.begin());

                return data.accepted;
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

    template<std::size_t N, typename matcher>
    struct tuple_element<N, e_regex::match_result<matcher>>
    {
            using type = std::string_view;
    };

    template<std::size_t N, typename matcher>
    auto get(e_regex::match_result<matcher>&& t) noexcept
    {
        return t.template get<N>();
    }

}// namespace std

#endif /* MATCH_RESULT */
