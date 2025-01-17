/*
 * Copyright (c) 2016-2021 Morwenn
 * SPDX-License-Identifier: MIT
 */
#ifndef CPPSORT_UTILITY_ITER_MOVE_H_
#define CPPSORT_UTILITY_ITER_MOVE_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <iterator>
#include <type_traits>
#include <utility>
#include "../detail/type_traits.h"

namespace cppsort
{
namespace utility
{
    ////////////////////////////////////////////////////////////
    // Generic iter_move and iter_swap

    namespace detail
    {
        template<typename Iterator>
        using iter_move_t = cppsort::detail::conditional_t<
            std::is_reference<typename std::iterator_traits<Iterator>::reference>::value,
            std::remove_reference_t<typename std::iterator_traits<Iterator>::reference>&&,
            std::decay_t<typename std::iterator_traits<Iterator>::reference>
        >;

        template<typename Iterator>
        using has_iter_move_t = decltype(iter_move(std::declval<Iterator&>()));

        template<typename Iterator>
        using has_iter_swap_t = decltype(iter_swap(
            std::declval<Iterator&>(), std::declval<Iterator&>())
        );
    }

    template<
        typename Iterator,
        typename = cppsort::detail::enable_if_t<
            cppsort::detail::is_detected_v<detail::has_iter_move_t, Iterator>
        >
    >
    constexpr auto iter_swap(Iterator lhs, Iterator rhs)
        -> void
    {
        auto tmp = iter_move(lhs);
        *lhs = iter_move(rhs);
        *rhs = std::move(tmp);
    }

    template<
        typename Iterator,
        typename = cppsort::detail::enable_if_t<
            not cppsort::detail::is_detected_v<detail::has_iter_move_t, Iterator>
        >,
        typename = void // dummy parameter for ODR
    >
    constexpr auto iter_swap(Iterator lhs, Iterator rhs)
        -> void
    {
        // While this overload is not strictly needed, it
        // ensures that an ADL-found swap is used when the
        // iterator type does not have a dedicated iter_move
        // ADL-found overload

        using std::swap;
        swap(*lhs, *rhs);
    }

    template<typename Iterator>
    constexpr auto iter_move(Iterator it)
        noexcept(noexcept(detail::iter_move_t<Iterator>(std::move(*it))))
        -> detail::iter_move_t<Iterator>
    {
        return std::move(*it);
    }

    ////////////////////////////////////////////////////////////
    // rvalue_reference_t type trait

    namespace adl_trick
    {
        using utility::iter_move;

        template<typename Iterator>
        auto do_iter_move()
            -> decltype(iter_move(std::declval<Iterator&>()));
    }

    template<typename Iterator>
    using rvalue_reference_t = decltype(adl_trick::do_iter_move<Iterator>());

    ////////////////////////////////////////////////////////////
    // std::reverse_iterator overloads

    template<typename Iterator>
    auto iter_move(const std::reverse_iterator<Iterator>& it)
        -> rvalue_reference_t<Iterator>
    {
        using utility::iter_move;
        return iter_move(std::prev(it.base()));
    }

    template<typename Iterator>
    auto iter_swap(std::reverse_iterator<Iterator> lhs, std::reverse_iterator<Iterator> rhs)
        -> void
    {
        using utility::iter_swap;
        iter_swap(std::prev(lhs.base()), std::prev(rhs.base()));
    }

    ////////////////////////////////////////////////////////////
    // std::move_iterator overloads

    template<typename Iterator>
    auto iter_move(const std::move_iterator<Iterator>& it)
        -> rvalue_reference_t<Iterator>
    {
        using utility::iter_move;
        return iter_move(it.base());
    }

    template<typename Iterator>
    auto iter_swap(std::move_iterator<Iterator> lhs, std::move_iterator<Iterator> rhs)
        -> void
    {
        using utility::iter_swap;
        iter_swap(lhs.base(), rhs.base());
    }
}}

#endif // CPPSORT_UTILITY_ITER_MOVE_H_
