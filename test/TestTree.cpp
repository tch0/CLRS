#include "TestUtil.hpp"
#include <Tree.hpp>
#include <random>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <iostream>

void testRedBlackTree(TestUtil& util)
{
    std::vector<int> vec(1000, 0);
    std::iota(vec.begin(), vec.end(), 0);
    std::shuffle(vec.begin(), vec.end(), std::mt19937());
    {
        // Multi = false
        using IntTree = RbTree<int, int, std::identity>;
        {
            IntTree tree;
            util.assertSorted(tree.begin(), tree.end());
            util.assertEqual(tree.size(), (std::size_t)0);
            util.assertEqual(tree.empty(), true);
        }
        {
            IntTree tree(vec.begin(), vec.end());
            util.assertSorted(tree.begin(), tree.end());
            util.assertEqual(tree.size(), (std::size_t)1000);
            util.assertSorted(tree.begin(), tree.end());
            // copy and move
            IntTree tree2(tree);
            util.assertEqual(tree.size(), (std::size_t)1000);
            util.assertEqual(tree2.size(), (std::size_t)1000);
            util.assertSorted(tree2.begin(), tree2.end());
            IntTree tree3(std::move(tree));
            util.assertEqual(tree.size(), (std::size_t)0);
            util.assertEqual(tree3.size(), (std::size_t)1000);
            util.assertSorted(tree2.begin(), tree2.end());
            // operator=
            IntTree tree4;
            tree4 = tree3;
            util.assertSorted(tree4.begin(), tree4.end());
            util.assertEqual(tree4.size(), (std::size_t)1000);
            tree3 = std::move(tree4);
            util.assertSorted(tree3.begin(), tree3.end());
            util.assertEqual(tree3.size(), (std::size_t)1000);
            util.assertEqual(tree4.size(), (std::size_t)0);

            // insert, emplace
            int a = 10, b = 1024;
            tree3.insert(1003);
            tree3.emplace(a); // duplicate
            tree3.emplace(b);
            tree3.insert(b); // duplicate
            util.assertSorted(tree3.begin(), tree3.end());
            util.assertEqual(tree3.size(), (std::size_t)1002);

            // erase
            for (int i = 200; i < 300; ++i)
            {
                auto iter = tree3.find(i);
                if (iter != tree3.end())
                {
                    tree3.erase(iter);
                }
            }
            util.assertSorted(tree3.begin(), tree3.end());
            util.assertEqual(tree3.size(), (std::size_t)902);

            // find
            auto iter = tree3.find(20);
            util.assertEqual(iter != tree3.end(), true);
            util.assertEqual(*iter, 20);

            // clear
            tree3.clear();
            util.assertEqual(tree3.size(), (std::size_t)0);
            util.assertEqual(tree3.empty(), true);
        }
    }
    {
        // Multi = true
        using IntTree = RbTree<int, int, std::identity, true, std::greater<>>;
        std::greater<> cmp;
        IntTree tree(vec.begin(), vec.end(), cmp);
        tree.insert(vec.begin(), vec.end());
        util.assertSorted(tree.begin(), tree.end(), cmp);
        util.assertEqual(tree.size(), (std::size_t)2000);
        for (int i = 200; i < 800; ++i)
        {
            auto iter = tree.find(i);
            if (iter != tree.end())
            {
                tree.erase(iter);
            }
        }
        util.assertEqual(tree.size(), (std::size_t)1400);
        util.assertSorted(tree.begin(), tree.end(), cmp);
        for (int i = 200; i < 800; ++i)
        {
            auto iter = tree.find(i);
            if (iter != tree.end())
            {
                tree.erase(iter);
            }
        }
        util.assertEqual(tree.size(), (std::size_t)800);
        util.assertSorted(tree.begin(), tree.end(), cmp);
    }
}

void testTree(DetailFlag detail)
{
    TestUtil util(detail, "tree");
    
    testRedBlackTree(util);

    util.showFinalResult();
}