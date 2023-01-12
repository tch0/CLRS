#pragma once
#include <iterator>

namespace CLRS
{

template<std::random_access_iterator RandomIterator, typename Compare = std::less<>>
void insertionSort(RandomIterator first, RandomIterator last, Compare comp = std::less<>())
{
    if (first == last || first + 1 == last)
        return;
    for (auto iter = first + 1; iter != last; ++iter)
    {
        auto key = std::move(*iter);
        auto iter2 = iter - 1;
        for (; iter2 != first && comp(key, *iter2); --iter2)
        {
            *(iter2+1) = *iter2;
        }
        *iter2 = std::move(key);
    }
}

} // namespace CLRS
