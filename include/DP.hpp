#pragma once
#include <iterator>
#include <algorithm>

namespace CLRS
{

// maximum subarray problem : find a subarray (of an array) that has the biggest summation
// dp[i] represent the maximum subarray summation that ends with input[i]
template<std::random_access_iterator RandomIterator>
typename std::iterator_traits<RandomIterator>::value_type maximumSubArray(RandomIterator first, RandomIterator last)
{
    using value_type = typename std::iterator_traits<RandomIterator>::value_type;
    if (first >= last)
    {
        return value_type{};
    }
    std::vector<value_type> dp(last - first);
    dp[0] = *first;
    value_type result = *first;
    for (auto iter = first + 1; iter < last; ++iter)
    {
        dp[iter-first] = std::max(*iter, *iter + dp[iter-first-1]);
        result = std::max(result, dp[iter-first]);
    }
    return result;
}

} // namespace CLRS


