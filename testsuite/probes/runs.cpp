/*
 * Copyright (c) 2016-2021 Morwenn
 * SPDX-License-Identifier: MIT
 */
#include <forward_list>
#include <vector>
#include <catch2/catch.hpp>
#include <cpp-sort/probes/runs.h>
#include <testing-tools/internal_compare.h>

TEST_CASE( "presortedness measure: runs", "[probe][runs]" )
{
    SECTION( "simple tests" )
    {
        std::forward_list<int> li = { 40, 49, 58, 99, 60, 70, 12, 87, 9, 8, 82, 91, 99, 67, 82, 92 };
        CHECK( cppsort::probe::runs(li) == 5 );
        CHECK( cppsort::probe::runs(li.begin(), li.end()) == 5 );

        // From Right invariant metrics and measures of
        // presortedness by Estivill-Castro, Mannila and Wood
        std::forward_list<int> li2 = { 4, 2, 6, 5, 3, 1, 9, 7, 10, 8 };
        CHECK( cppsort::probe::runs(li2) == 6 );
        CHECK( cppsort::probe::runs(li2.begin(), li2.end()) == 6 );

        std::vector<internal_compare<int>> tricky(li.begin(), li.end());
        CHECK( cppsort::probe::runs(tricky, &internal_compare<int>::compare_to) == 5 );
    }

    SECTION( "upper bound" )
    {
        // The upper bound should correspond to the size of
        // the input sequence minus one

        std::forward_list<int> li = { 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
        CHECK( cppsort::probe::runs(li) == 10 );
        CHECK( cppsort::probe::runs(li.begin(), li.end()) == 10 );
    }
}
