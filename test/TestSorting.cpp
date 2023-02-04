#include "TestUtil.hpp"
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>
#include <string>
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
            std::copy(vec.begin(), vec.end(), std::back_inserter(tmp));
            std::vector<int> tmp2(tmp);
            CLRS::insertionSort(tmp.begin(), tmp.end(), std::greater<>());
            util.assertSorted(tmp.begin(), tmp.end(), std::greater<>());
            util.assertEqual(std::is_sorted(tmp.begin(), tmp.end(), std::greater<>()), true);
            
            std::sort(tmp2.begin(), tmp2.end(), std::greater<>());
            util.assertSequenceEqual(tmp, tmp2);
        }
        {
            std::vector<std::string> tmp;
            tmp.reserve(vec.size() * 2);
            std::transform(vec.begin(), vec.end(), std::back_inserter(tmp), [](int val) -> std::string { return std::to_string(val); });
            std::copy(tmp.begin(), tmp.end(), std::back_inserter(tmp));
            std::vector<std::string> tmp2(tmp);
            CLRS::insertionSort(tmp.begin(), tmp.end(), std::less<>());
            util.assertSorted(tmp.begin(), tmp.end(), std::less<>());
            util.assertEqual(std::is_sorted(tmp.begin(), tmp.end(), std::less<>()), true);
            
            std::sort(tmp2.begin(), tmp2.end(), std::less<>());
            util.assertSequenceEqual(tmp, tmp2);
        }
    }
    {
        // merge sort
        {
            std::vector<int> tmp(vec);
            CLRS::mergeSort(tmp.begin(), tmp.end());
            util.assertSorted(tmp.begin(), tmp.end());
            util.assertEqual(std::is_sorted(tmp.begin(), tmp.end()), true);
        }
        {
            std::vector<int> tmp(vec);
            std::copy(vec.begin(), vec.end(), std::back_inserter(tmp));
            std::vector<int> tmp2(tmp);
            CLRS::mergeSort(tmp.begin(), tmp.end(), std::greater<>());
            util.assertSorted(tmp.begin(), tmp.end(), std::greater<>());
            util.assertEqual(std::is_sorted(tmp.begin(), tmp.end(), std::greater<>()), true);

            std::sort(tmp2.begin(), tmp2.end(), std::greater<>());
            util.assertSequenceEqual(tmp, tmp2);
        }
        {
            std::vector<std::string> tmp;
            tmp.reserve(vec.size() * 2);
            std::transform(vec.begin(), vec.end(), std::back_inserter(tmp), [](int val) -> std::string { return std::to_string(val); });
            std::copy(tmp.begin(), tmp.end(), std::back_inserter(tmp));
            std::vector<std::string> tmp2(tmp);
            CLRS::mergeSort(tmp.begin(), tmp.end(), std::less<>());
            util.assertSorted(tmp.begin(), tmp.end(), std::less<>());
            util.assertEqual(std::is_sorted(tmp.begin(), tmp.end(), std::less<>()), true);
            
            std::sort(tmp2.begin(), tmp2.end(), std::less<>());
            util.assertSequenceEqual(tmp, tmp2);
        }
    }
    {
        // heap sort
        {
            std::vector<int> tmp(vec);
            CLRS::heapSort(tmp.begin(), tmp.end());
            util.assertSorted(tmp.begin(), tmp.end());
            util.assertEqual(std::is_sorted(tmp.begin(), tmp.end()), true);
        }
        {
            std::vector<int> tmp(vec);
            std::copy(vec.begin(), vec.end(), std::back_inserter(tmp));
            std::vector<int> tmp2(tmp);
            CLRS::heapSort(tmp.begin(), tmp.end(), std::greater<>());
            util.assertSorted(tmp.begin(), tmp.end(), std::greater<>());
            util.assertEqual(std::is_sorted(tmp.begin(), tmp.end(), std::greater<>()), true);

            std::sort(tmp2.begin(), tmp2.end(), std::greater<>());
            util.assertSequenceEqual(tmp, tmp2);
        }
        {
            std::vector<std::string> tmp;
            tmp.reserve(vec.size() * 2);
            std::transform(vec.begin(), vec.end(), std::back_inserter(tmp), [](int val) -> std::string { return std::to_string(val); });
            std::copy(tmp.begin(), tmp.end(), std::back_inserter(tmp));
            std::vector<std::string> tmp2(tmp);
            CLRS::heapSort(tmp.begin(), tmp.end(), std::less<>());
            util.assertSorted(tmp.begin(), tmp.end(), std::less<>());
            util.assertEqual(std::is_sorted(tmp.begin(), tmp.end(), std::less<>()), true);
            
            std::sort(tmp2.begin(), tmp2.end(), std::less<>());
            util.assertSequenceEqual(tmp, tmp2);
        }
    }
    {
        // quick sort
        {
            std::vector<int> tmp(vec);
            CLRS::quickSort(tmp.begin(), tmp.end());
            util.assertSorted(tmp.begin(), tmp.end());
            util.assertEqual(std::is_sorted(tmp.begin(), tmp.end()), true);
        }
        {
            std::vector<int> tmp(vec);
            std::copy(vec.begin(), vec.end(), std::back_inserter(tmp));
            std::vector<int> tmp2(tmp);
            CLRS::quickSort(tmp.begin(), tmp.end(), std::greater<>());
            util.assertSorted(tmp.begin(), tmp.end(), std::greater<>());
            util.assertEqual(std::is_sorted(tmp.begin(), tmp.end(), std::greater<>()), true);

            std::sort(tmp2.begin(), tmp2.end(), std::greater<>());
            util.assertSequenceEqual(tmp, tmp2);
        }
        {
            std::vector<std::string> tmp;
            tmp.reserve(vec.size() * 2);
            std::transform(vec.begin(), vec.end(), std::back_inserter(tmp), [](int val) -> std::string { return std::to_string(val); });
            std::copy(tmp.begin(), tmp.end(), std::back_inserter(tmp));
            std::vector<std::string> tmp2(tmp);
            CLRS::quickSort(tmp.begin(), tmp.end(), std::less<>());
            util.assertSorted(tmp.begin(), tmp.end(), std::less<>());
            util.assertEqual(std::is_sorted(tmp.begin(), tmp.end(), std::less<>()), true);
            
            std::sort(tmp2.begin(), tmp2.end(), std::less<>());
            util.assertSequenceEqual(tmp, tmp2);
        }
    }
    {
        // counting sort
        {
            std::vector<int> tmp(vec);
            CLRS::countingSort(tmp.begin(), tmp.end(), 1000);
            util.assertSorted(tmp.begin(), tmp.end());
            util.assertEqual(std::is_sorted(tmp.begin(), tmp.end()), true);
        }
        {
            std::vector<int> tmp(vec);
            std::copy(vec.begin(), vec.end(), std::back_inserter(tmp));
            std::vector<int> tmp2(tmp);
            CLRS::countingSort(tmp.begin(), tmp.end(), 1000);
            util.assertSorted(tmp.begin(), tmp.end());
            util.assertEqual(std::is_sorted(tmp.begin(), tmp.end()), true);
            
            std::sort(tmp2.begin(), tmp2.end());
            util.assertSequenceEqual(tmp, tmp2);
        }
    }
    {
        
    }
    util.showFinalResult();
}