#include "TestUtil.hpp"
#include <vector>
#include <iterator>
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
    util.showFinalResult();
}