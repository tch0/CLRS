#include "TestUtil.hpp"
#include <vector>
#include <iterator>
#include <vector>
#include <sstream>
#include <DP.hpp>

void testDP(DetailFlag detail)
{
    TestUtil util(detail, "DP");
    {
        // maximum subarray
        std::vector<int> vec{12, -3, -25, 20, -3, -16, -23, 18, 20, -7, 12, -5, -22, 15, -4, 7};
        auto result = CLRS::maximumSubArray(vec.begin(), vec.end());
        util.assertEqual(result, 43);
    }
    {
        // minimum calculation times of matrix chain order
        std::vector<int> vec{30, 35, 15, 5, 10, 20, 25};
        auto res = CLRS::minimumMatrixChainOrder(vec.begin(), vec.end());
        util.assertEqual(CLRS::optimalMatrixChainOrderAsString(res.second, 0, 5), "((A0(A1A2))((A3A4)A5))");
        util.assertEqual(res.first[0][5], 15125);
    }
    {
        // longest common subsequence (LCS) problem
        std::string sa = "ABCBDAB";
        std::string sb = "BDCABA";
        // LCS will be BCBA or BCAB or BDAB
        auto res = CLRS::LCS(sa.begin(), sa.end(), sb.begin(), sb.end());
        std::string slcs;
        util.assertEqual(res.first[sa.size()][sb.size()], std::size_t(4));
        CLRS::LCSResultToSubsequence(sa.begin(), sa.end(), res.second, std::back_inserter(slcs), sa.size(), sb.size());
        util.assertEqual(slcs == "BCBA" || slcs == "BCAB" || slcs == "BDAB", true);
    }
    util.showFinalResult();
}