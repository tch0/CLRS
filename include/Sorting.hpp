#pragma once
#include <iterator>
#include <vector>
#include <cassert>

namespace CLRS
{

// insertion sort
// time complexity: best O(N), wrost O(N^2), average O(N^2)
// space complexity: O(1), inplace sorting
template<std::random_access_iterator RandomIterator, typename Compare = std::less<>>
void insertionSort(RandomIterator first, RandomIterator last, Compare comp = std::less<>())
{
    assert(first < last);
    if (first == last || first + 1 == last)
        return;
    for (auto iter = first + 1; iter != last; ++iter)
    {
        auto key = std::move(*iter);
        auto iter2 = iter;
        for (; iter2 != first && comp(key, *(iter2-1)); --iter2)
        {
            *iter2 = *(iter2-1);
        }
        *iter2 = std::move(key);
    }
}

// merge sort : a divide and conquer algorithm
// time complexity: average O(NlogN)
// space complexity: O(N)
namespace Impl
{
template<std::random_access_iterator RandomIterator1, std::random_access_iterator RandomIterator2, typename Compare = std::less<>>
void mergeSortImpl(RandomIterator1 sFirst, RandomIterator1 sLast, RandomIterator2 dFirst, Compare comp = std::less<>())
{
    assert(sFirst < sLast);
    if (sLast - sFirst <= 1)
        return;
    std::size_t offset = (sLast - sFirst) / 2;
    mergeSortImpl(sFirst, sFirst + offset, dFirst, comp);
    mergeSortImpl(sFirst + offset, sLast, dFirst + offset, comp);
    std::merge(std::make_move_iterator(sFirst), std::make_move_iterator(sFirst + offset),
               std::make_move_iterator(sFirst + offset), std::make_move_iterator(sLast),
               dFirst, comp);
    std::move(dFirst, dFirst + (sLast - sFirst), sFirst);
}
} // namespace Impl

template<std::random_access_iterator RandomIterator, typename Compare = std::less<>>
void mergeSort(RandomIterator first, RandomIterator last, Compare comp = std::less<>())
{
    assert(first < last);
    if (last - first <= 1)
        return;
    using value_type = std::iterator_traits<RandomIterator>::value_type;
    std::vector<value_type> tmpVec;
    tmpVec.reserve(last - first);
    std::move(first, last, std::back_inserter(tmpVec));
    Impl::mergeSortImpl(tmpVec.begin(), tmpVec.end(), first, comp);
    std::move(tmpVec.begin(), tmpVec.end(), first);
}

// heap sort
// time complexity: O(NlogN)
// space complexity: O(1), inplace sorting
template<std::random_access_iterator RandomIterator, typename Compare = std::less<>>
void heapSort(RandomIterator first, RandomIterator last, Compare comp = std::less<>())
{
    std::make_heap(first, last, comp);
    std::sort_heap(first, last, comp);
}

// quick sort
// time complexity: average O(NlogN), wrost O(N^2)
// space complexity: O(1), inplace sorting
template<std::random_access_iterator RandomIterator, typename Compare = std::less<>>
void quickSort(RandomIterator first, RandomIterator last, Compare comp = std::less<>())
{
    if (last - first <= 1)
        return;
    auto pivot = *(first + (last-first) / 2);
    auto middle1 = std::partition(first, last, [pivot, &comp](const auto& elem) { return comp(elem, pivot); });
    auto middle2 = std::partition(middle1, last, [pivot, &comp](const auto& elem) { return !comp(pivot, elem); });
    quickSort(first, middle1, comp);
    quickSort(middle2, last, comp);
}

// counting sort
// time complexity: O(k+N), k is the upper bound of the range, N is input scale
template<std::random_access_iterator RandomIterator> requires std::integral<typename std::iterator_traits<RandomIterator>::value_type>
void countingSort(RandomIterator first, RandomIterator last, int K)
{
    std::vector<int> vec(K+1, 0);
    for (auto iter = first; iter < last; ++iter)
    {
        vec[*iter]++;
    }
    for (int i = K; i >= 0;)
    {
        if (vec[i] > 0)
        {
            *(--last) = i;
            vec[i]--;
        }
        else
        {
            --i;
        }
    }
}

} // namespace CLRS
