/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2020 Morwenn
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef CPPSORT_PROBES_EXC_H_
#define CPPSORT_PROBES_EXC_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>
#include <cpp-sort/sorter_facade.h>
#include <cpp-sort/sorter_traits.h>
#include <cpp-sort/utility/as_function.h>
#include <cpp-sort/utility/functional.h>
#include <cpp-sort/utility/size.h>
#include <cpp-sort/utility/static_const.h>
#include "../detail/indirect_compare.h"
#include "../detail/iterator_traits.h"
#include "../detail/pdqsort.h"

namespace cppsort
{
namespace probe
{
    namespace detail
    {
        template<typename ForwardIterator, typename Compare, typename Projection>
        auto exc_probe_algo(ForwardIterator first, ForwardIterator last,
                            cppsort::detail::difference_type_t<ForwardIterator> size,
                            Compare compare, Projection projection)
            -> ::cppsort::detail::difference_type_t<ForwardIterator>
        {
            using difference_type = ::cppsort::detail::difference_type_t<ForwardIterator>;
            auto&& comp = utility::as_function(compare);
            auto&& proj = utility::as_function(projection);

            if (size < 2) {
                return 0;
            }

            ////////////////////////////////////////////////////////////
            // Indirectly sort the iterators

            // Copy the iterators in a vector
            std::vector<ForwardIterator> iterators;
            iterators.reserve(size);
            for (auto it = first ; it != last ; ++it) {
                iterators.push_back(it);
            }

            // Sort the iterators on pointed values
            pdqsort(
                iterators.begin(), iterators.end(),
                cppsort::detail::make_indirect_compare(compare, projection),
                utility::identity{}
            );

            ////////////////////////////////////////////////////////////
            // Count the number of cycles

            std::vector<bool> sorted(size, false);

            // Element where the current cycle starts
            auto start = first;

            difference_type cycles = 0;
            while (start != last) {
                // Find the element to put in current's place
                auto current = start;
                auto next_pos = std::distance(first, current);
                auto next = iterators[next_pos];
                sorted[next_pos] = true;

                // Process the current cycle
                if (next != current) {
                    while (next != start) {
                        // If an element is in the place of another element that compares
                        // equivalent, it means that this element was actually already in
                        // a suitable place, so we count one more cycle as if it was an
                        // already suitably placed element, this handles collections with
                        // several elements which compare equivalent
                        if (not comp(proj(*next), proj(*current)) &&
                            not comp(proj(*current), proj(*next))) {
                            ++cycles;
                        }
                        // Locate the next element of the cycle
                        current = next;
                        auto next_pos = std::distance(first, next);
                        next = iterators[next_pos];
                        sorted[next_pos] = true;
                    }
                }

                ++cycles;

                // Find the next cycle
                auto&& sorted_it = sorted.begin() + std::distance(first, start);
                do {
                    ++start;
                    ++sorted_it;
                } while (start != last && *sorted_it);
            }
            return size - cycles;
        }

        struct exc_impl
        {
            template<
                typename ForwardIterable,
                typename Compare = std::less<>,
                typename Projection = utility::identity,
                typename = std::enable_if_t<
                    is_projection_v<Projection, ForwardIterable, Compare>
                >
            >
            auto operator()(ForwardIterable&& iterable,
                            Compare compare={}, Projection projection={}) const
                -> decltype(auto)
            {
                return exc_probe_algo(std::begin(iterable), std::end(iterable),
                                      utility::size(iterable),
                                      std::move(compare), std::move(projection));
            }

            template<
                typename ForwardIterator,
                typename Compare = std::less<>,
                typename Projection = utility::identity,
                typename = std::enable_if_t<
                    is_projection_iterator_v<Projection, ForwardIterator, Compare>
                >
            >
            auto operator()(ForwardIterator first, ForwardIterator last,
                            Compare compare={}, Projection projection={}) const
                -> decltype(auto)
            {
                return exc_probe_algo(first, last, std::distance(first, last),
                                      std::move(compare), std::move(projection));
            }
        };
    }

    namespace
    {
        constexpr auto&& exc = utility::static_const<
            sorter_facade<detail::exc_impl>
        >::value;
    }
}}

#endif // CPPSORT_PROBES_EXC_H_
