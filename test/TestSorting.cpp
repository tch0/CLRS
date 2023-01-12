#include "TestUtil.hpp"
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>
#include <Sorting.hpp>

void testSorting(DetailFlag detail)
{
    TestUtil util(detail, "sorting");
    std::vector<int> vec(1000, 0);
    std::iota(vec.begin(), vec.end(), 0);
    std::shuffle(vec.begin(), vec.end(), std::mt19937());
    {
        // insertion sort
        {
            std::vector<int> tmp(vec);
            CLRS::insertionSort(tmp.begin(), tmp.end());
            util.assertSorted(tmp.begin(), tmp.end());
            util.assertEqual(std::is_sorted(tmp.begin(), tmp.end()), true);
        }
        {
            std::vector<int> tmp(vec);
            std::copy(tmp.begin(), tmp.end(), std::back_inserter(vec));
            CLRS::insertionSort(tmp.begin(), tmp.end(), std::greater<>());
            util.assertSorted(tmp.begin(), tmp.end(), std::greater<>());
            util.assertEqual(std::is_sorted(tmp.begin(), tmp.end(), std::greater<>()), true);
        }
    }
    util.showFinalResult();
}