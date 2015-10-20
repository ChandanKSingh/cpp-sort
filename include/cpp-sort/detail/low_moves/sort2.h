/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Morwenn
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
#ifndef CPPSORT_DETAIL_LOW_MOVES_SORT2_H_
#define CPPSORT_DETAIL_LOW_MOVES_SORT2_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <utility>
#include <cpp-sort/sorter_facade.h>

namespace cppsort
{
    template<>
    struct low_moves_sorter<2u>:
        sorter_facade<low_moves_sorter<2u>>
    {
        using sorter_facade<low_moves_sorter<2u>>::operator();

        template<typename RandomAccessIterator, typename Compare>
        auto operator()(RandomAccessIterator first, RandomAccessIterator, Compare compare) const
            -> void
        {
            using std::swap;

            if (compare(first[1u], first[0u])) {
                swap(first[0u], first[1u]);
            }
        }
    };
}

#endif // CPPSORT_DETAIL_LOW_MOVES_SORT2_H_
