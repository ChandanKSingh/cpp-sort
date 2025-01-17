/*
 * Copyright (c) 2019-2021 Morwenn
 * SPDX-License-Identifier: MIT
 */
#ifndef CPPSORT_DETAIL_MOVE_H_
#define CPPSORT_DETAIL_MOVE_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <algorithm>
#include <iterator>
#include <type_traits>
#include <cpp-sort/utility/iter_move.h>
#include "iterator_traits.h"
#include "memory.h"
#include "type_traits.h"

namespace cppsort
{
namespace detail
{
    ////////////////////////////////////////////////////////////
    // Check whether iter_move is specialized
    //
    // The idea behind this check is that if an iterator has a
    // dedicated iter_move found by ADL, then we ought to use it,
    // otherwise we can fallback to std::move which is supposedly
    // more optimized than what we would write by hand (notably
    // for standard library iterators)
    //

    namespace hide_adl
    {
        template<typename Iterator>
        auto iter_move(Iterator) = delete;

        struct dummy_callable
        {
            template<typename Iterator>
            auto operator()(Iterator it)
                -> decltype(iter_move(it));

            template<typename Iterator>
            auto operator()(const std::reverse_iterator<Iterator>& it)
                -> decltype(iter_move(it.base())); // only there for type information

            template<typename Iterator>
            auto operator()(const std::move_iterator<Iterator>& it)
                -> decltype(iter_move(it.base()));
        };
    }

    ////////////////////////////////////////////////////////////
    // move

    template<typename InputIterator, typename OutputIterator>
    auto move(InputIterator first, InputIterator last, OutputIterator result)
        -> detail::enable_if_t<
            not is_invocable_v<hide_adl::dummy_callable, InputIterator>,
            OutputIterator
        >
    {
        return std::move(first, last, result);
    }

    template<typename InputIterator, typename OutputIterator>
    auto move(InputIterator first, InputIterator last, OutputIterator result)
        -> detail::enable_if_t<
            is_invocable_v<hide_adl::dummy_callable, InputIterator>,
            OutputIterator
        >
    {
        for (; first != last; ++first, (void) ++result) {
            using utility::iter_move;
            *result = iter_move(first);
        }
        return result;
    }

    ////////////////////////////////////////////////////////////
    // move_backward

    template<typename InputIterator, typename OutputIterator>
    auto move_backward(InputIterator first, InputIterator last, OutputIterator result)
        -> detail::enable_if_t<
            not is_invocable_v<hide_adl::dummy_callable, InputIterator>,
            OutputIterator
        >
    {
        return std::move_backward(first, last, result);
    }

    template<typename InputIterator, typename OutputIterator>
    auto move_backward(InputIterator first, InputIterator last, OutputIterator result)
        -> detail::enable_if_t<
            is_invocable_v<hide_adl::dummy_callable, InputIterator>,
            OutputIterator
        >
    {
        while (first != last) {
            using utility::iter_move;
            *--result = iter_move(--last);
        }
        return result;
    }

    ////////////////////////////////////////////////////////////
    // uninitialized_move

    template<typename InputIterator, typename T>
    auto uninitialized_move_impl(std::true_type, InputIterator first, InputIterator last,
                                 T* result, destruct_n<T>&)
        -> T*
    {
        return detail::move(first, last, result);
    }

    template<typename InputIterator, typename T>
    auto uninitialized_move_impl(std::false_type, InputIterator first, InputIterator last,
                                 T* result, destruct_n<T>& destroyer)
        -> T*
    {
        for (; first != last; ++first, (void) ++result, ++destroyer) {
            using utility::iter_move;
            ::new(static_cast<void*>(result)) T(iter_move(first));
        }
        return result;
    }

    template<typename InputIterator, typename T>
    auto uninitialized_move(InputIterator first, InputIterator last, T* result, destruct_n<T>& destroyer)
        -> T*
    {
        using truth_type = std::integral_constant<bool,
            std::is_trivial<value_type_t<InputIterator>>::value &&
            std::is_trivial<T>::value
        >;
        return uninitialized_move_impl(truth_type{}, std::move(first), std::move(last),
                                       std::move(result), destroyer);
    }
}}

#endif // CPPSORT_DETAIL_MOVE_H_
