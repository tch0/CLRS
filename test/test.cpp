#include "TestUtil.hpp"
#include <vector>
#include <iterator>
#include <algorithm>
#include <numeric>

void testDivideAndConquer(DetailFlag detail);
void testSorting(DetailFlag detail);
void testDataStructure(DetailFlag detail);
void testDP(DetailFlag detail);
void testGreedy(DetailFlag detail);
void testTree(DetailFlag detail);
void testGraph(DetailFlag detail);
void testMatrix(DetailFlag detail);
void testLinearProgramming(DetailFlag detail);
void testFFT(DetailFlag detail);
void testNumberTheory(DetailFlag detail);
void testString(DetailFlag detail);
void testGeometry(DetailFlag detail);

int main(int argc, char const *argv[])
{
    auto detail = parseDetailFlag(argc, argv);
    testDivideAndConquer(detail);
    testSorting(detail);
    testDataStructure(detail);
    testDP(detail);
    testGreedy(detail);
    testTree(detail);
    testGraph(detail);
    testMatrix(detail);
    testLinearProgramming(detail);
    testFFT(detail);
    testNumberTheory(detail);
    testString(detail);
    testGeometry(detail);
    return 0;
}
