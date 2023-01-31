#pragma once
#include <vector>
#include <iterator>
#include <algorithm>
#include <concepts>
#include <numeric>
#include <cassert>

namespace CLRS
{

// maximum subarray problem : find a subarray (of an array) that has the biggest summation
// dp[i] represent the maximum subarray summation that ends with input[i]
// time complexity: O(N)
// space complexity: O(N)
template<std::random_access_iterator RandomIterator>
typename std::iterator_traits<RandomIterator>::value_type maximumSubArray(RandomIterator first, RandomIterator last)
{
    using value_type = typename std::iterator_traits<RandomIterator>::value_type;
    if (first >= last)
    {
        return value_type{};
    }
    std::size_t N = last - first;
    std::vector<value_type> dp(N);
    dp[0] = *first;
    value_type result = *first;
    for (std::size_t i = 2; i < N; ++i)
    {
        dp[i] = std::max(first[i], first[i] + dp[i-1]);
        result = std::max(result, dp[i]);
    }
    return result;
}

// minimum calculation times of matrix chain order
// input is the list of row of first matrix, column of first matirx, row of second matrix, ..., column of last matrix
//       it's size is n+1, n is the number of matrices
// m[i,j] represent the minimum calculation times of Ai * ... * Aj (i <= j)
//      m[i,j] = 0,                                           if i == j
//             = min{m[i,k] + m[k+1,j] + p[i]*p[k+1]*p[j+1]}, if i < j, for every i<=k<j.
// time complexity: O(N^3)
// space complexity: O(N^2)
template<std::random_access_iterator RandomIterator> requires std::integral<typename std::iterator_traits<RandomIterator>::value_type>
auto minimumMatrixChainOrder(RandomIterator first, RandomIterator last)
{
    using value_type = typename std::iterator_traits<RandomIterator>::value_type;
    std::size_t N = last - first - 1; // number of matrices
    auto p = first;
    std::vector<std::vector<value_type>> m(N, std::vector<value_type>(N));
    std::vector<std::vector<value_type>> s(N, std::vector<value_type>(N));
    for (std::size_t i = 0; i < N; ++i)
    {
        m[i][i] = 0;
    }
    for (std::size_t len = 2; len <= N; ++len) // len is the chain length.
    {
        for (std::size_t i = 0; i < N-len+1; ++i)
        {
            std::size_t j = i+len-1;
            m[i][j] = std::numeric_limits<value_type>::max(); // initialize to +infinity(represented by max value of the type)
            for (std::size_t k = i; k <= j-1; ++k)
            {
                auto val = m[i][k] + m[k+1][j] + p[i]*p[k+1]*p[j+1];
                if (val < m[i][j])
                {
                    m[i][j] = val;
                    s[i][j] = k;
                }
            }
        }
    }
    return std::make_pair(std::move(m), std::move(s));
}
// for minimum matrix chain order problem
template<std::integral T>
std::string optimalMatrixChainOrderAsString(const std::vector<std::vector<T>>& s, std::size_t i, std::size_t j)
{
    std::ostringstream oss;
    if (i == j)
    {
        oss << "A" << i;
    }
    else
    {
        oss << "(";
        oss << optimalMatrixChainOrderAsString(s, i, s[i][j]);
        oss << optimalMatrixChainOrderAsString(s, s[i][j]+1, j);
        oss << ")";
    }
    return oss.str();
}

// longest common subsequence (LCS) problem
// example: for X = <A,B,C,B,D,A,B>, Y = <B,D,C,A,B,A>, LCS are <B,C,B,A> or <B,C,A,B> or <B,D,A,B>
// for sequences X and Y, define Xi and Yi as the subsequence of first i-elements of X and Y (index from 1 !!!)
// define c[i,j] as the maximum subsequence length of Xi and Yi
// so c[i,j] = 0,                       if i == 0 or j == 0 (empty subsequence)
//           = c[i-1,j-1] + 1,          if i,j > 0 and xi == yi
//           = max(c[i,j-1],c[i-1,j]),  if i,j > 0 and xi != yi
// use a 2d-array b save the result informations: construct the subsequence result from b
//       0 : xi == yi
//      -1 : xi != yi, c[i,j-1] >= c[i-1,j], the LCS of Xi & Yj is the LCS of Xi & Yj-1
//       1 : xi != yi, c[i,j-1] < c[i-1,j],  the LCS of Xi & Yj is the LCS of Xi-1 & Yj
// time complexity: O(xlen+ylen)
// space complexity: O(xlen*ylen)
// note: the table b could be optimized, it's enough to construct the result lcs using table c only.
template<std::random_access_iterator RandomIterator1, std::random_access_iterator RandomIterator2>
auto LCS(RandomIterator1 firstx, RandomIterator1 lastx, RandomIterator2 firsty, RandomIterator2 lasty)
{
    using value_type = typename std::iterator_traits<RandomIterator1>::value_type;
    using value_type2 = typename std::iterator_traits<RandomIterator2>::value_type;
    static_assert(std::convertible_to<value_type2, value_type>);
    std::size_t xlen = lastx - firstx;
    std::size_t ylen = lasty - firsty;
    // attention: index is start from 1 in b & c for convenience, but in input sequences, they are started from 0.
    std::vector<std::vector<int>> b(xlen+1, std::vector<int>(ylen+1)); // to construct the longest subsequence
    std::vector<std::vector<std::size_t>> c(xlen+1, std::vector<std::size_t>(ylen+1));
    for (std::size_t i = 0; i <= xlen; ++i)
    {
        c[i][0] = 0;
    }
    for (std::size_t i = 0; i <= ylen; ++i)
    {
        c[0][i] = 0;
    }
    for (std::size_t i = 1; i <= xlen; ++i)
    {
        for (std::size_t j = 1; j <= ylen; ++j)
        {
            if (firstx[i-1] == firsty[j-1]) // compare xi and yj
            {
                c[i][j] = c[i-1][j-1] + 1;
                b[i][j] = 0;
            }
            else if (c[i-1][j] > c[i][j-1]) // Xi-1 & Yj
            {
                c[i][j] = c[i-1][j];
                b[i][j] = 1;
            }
            else // Xi & Yj-1
            {
                c[i][j] = c[i][j-1];
                b[i][j] = -1;
            }
        }
    }
    return std::make_pair(std::move(c), std::move(b));
}
// get subsequence from LCS function result
template<std::random_access_iterator RandomIterator, std::output_iterator<typename std::iterator_traits<RandomIterator>::value_type> OutputIterator>
void LCSResultToSubsequence(RandomIterator firstx, RandomIterator lastx, const std::vector<std::vector<int>>& b, OutputIterator outIter, std::size_t i, std::size_t j)
{
    // attention: i,j and indexes in b are started from 1.
    [[maybe_unused]] std::size_t xlen = lastx - firstx;
    assert(i <= xlen);
    assert(i < b.size());
    assert(j < b[0].size());
    if (i == 0 || j == 0)
    {
        return;
    }
    if (b[i][j] == 0) // xi == yj
    {
        LCSResultToSubsequence(firstx, lastx, b, outIter, i-1, j-1);
        *outIter++ = firstx[i-1];
    }
    else if (b[i][j] == 1) // Xi-1 & Yj
    {
        LCSResultToSubsequence(firstx, lastx, b, outIter, i-1, j);
    }
    else // Xi & Yj-1
    {
        LCSResultToSubsequence(firstx, lastx, b, outIter, i, j-1);
    }
}

} // namespace CLRS


