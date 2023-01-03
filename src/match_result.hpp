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

    template<typename matcher, std::size_t groups>
    class match_result
    {
            friend matcher;

        private:
            match_result_data<groups> data;
            bool                      initialized = false;

            constexpr void init()
            {
                initialized              = true;
                data.matches             = 0;
                data.match_groups        = {};
                data.actual_iterator_end = data.actual_iterator_start;
                data.accepted            = true;

                data = matcher::match(data.actual_iterator_start, data.query.end(), std::move(data));
            }

        public:
            constexpr match_result(std::string_view query)
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

            constexpr auto get_group(std::size_t index) const noexcept
            {
                if (index == 0)
                {
                    return to_view();
                }

                return data.match_groups[index - 1];
            }

            constexpr auto to_view() const noexcept
            {
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
                        // If accepted is false, probably this is the first call
                        data.actual_iterator_start++;
                    }

                    init();
                } while (!data.accepted && data.actual_iterator_start < data.query.end());

                return data.accepted;
            }
    };
}// namespace e_regex

#endif /* MATCH_RESULT */
