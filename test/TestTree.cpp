#include "TestUtil.hpp"
#include <Tree.hpp>
#include <random>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <iostream>

void testBinarySearchTree(TestUtil& util)
{
    std::vector<int> vec(1000, 0);
    std::iota(vec.begin(), vec.end(), 0);
    std::shuffle(vec.begin(), vec.end(), std::mt19937());
    {
        // Multi = false
        using IntTree = CLRS::BsTree<int, int, std::identity>;
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
            util.assertSorted(tree3.begin(), tree3.end());
            // operator=
            IntTree tree4;
            tree4 = tree3;
            util.assertSorted(tree4.begin(), tree4.end());
            util.assertEqual(tree4.size(), (std::size_t)1000);
            tree3 = std::move(tree4);
            util.assertSorted(tree3.begin(), tree3.end());
            util.assertEqual(tree3.size(), (std::size_t)1000);
            util.assertEqual(tree4.size(), (std::size_t)0);
            // iterators
            auto it = tree3.begin();
            util.assertEqual(*it, 0);
            util.assertEqual(*it++, 0);
            util.assertEqual(*it, 1);
            util.assertEqual(*++it, 2);
            util.assertEqual(*it--, 2);
            util.assertEqual(*--it, 0);

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
        using IntTree = CLRS::BsTree<int, int, std::identity, true, std::greater<>>;
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
        util.assertSorted(std::make_reverse_iterator(tree.end()), std::make_reverse_iterator(tree.begin()));
    }
}

void testRedBlackTree(TestUtil& util)
{
    std::vector<int> vec(1000, 0);
    std::iota(vec.begin(), vec.end(), 0);
    std::shuffle(vec.begin(), vec.end(), std::mt19937());
    {
        // Multi = false
        using IntTree = CLRS::RbTree<int, int, std::identity>;
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
            util.assertSorted(tree3.begin(), tree3.end());
            // operator=
            IntTree tree4;
            tree4 = tree3;
            util.assertSorted(tree4.begin(), tree4.end());
            util.assertEqual(tree4.size(), (std::size_t)1000);
            tree3 = std::move(tree4);
            util.assertSorted(tree3.begin(), tree3.end());
            util.assertEqual(tree3.size(), (std::size_t)1000);
            util.assertEqual(tree4.size(), (std::size_t)0);
            // iterators
            auto it = tree3.begin();
            util.assertEqual(*it, 0);
            util.assertEqual(*it++, 0);
            util.assertEqual(*it, 1);
            util.assertEqual(*++it, 2);
            util.assertEqual(*it--, 2);
            util.assertEqual(*--it, 0);

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
        using IntTree = CLRS::RbTree<int, int, std::identity, true, std::greater<>>;
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
        util.assertSorted(std::make_reverse_iterator(tree.end()), std::make_reverse_iterator(tree.begin()));
    }
}

void testTreap(TestUtil& util)
{
    std::random_device rd;
    std::vector<int> keys(1000, 0);
    std::vector<int> priorities(1000, 0);
    std::iota(keys.begin(), keys.end(), 0);
    std::iota(priorities.begin(), priorities.end(), 0);
    // different seed for keys and priorities
    std::shuffle(keys.begin(), keys.end(), std::mt19937(0));
    std::shuffle(priorities.begin(), priorities.end(), std::mt19937(1));

    {
        auto getFirst = [](std::pair<int, int> p) -> int { return p.first; }; // return value
        auto getSecond = [](const std::pair<int, int>& p) -> const int& { return p.second; }; // return reference
        auto cmpFirst = [](const std::pair<int, int>& p1, const std::pair<int, int>& p2) -> bool {
            return p1.first < p2.first;
        };
        using IntTreap = CLRS::Treap<int, int, std::pair<int, int>, decltype(getFirst), decltype(getSecond)>;
        IntTreap tree;
        for (std::size_t i = 0; i < keys.size(); ++i)
        {
            tree.emplace(keys[i], priorities[i]);
        }
        util.assertEqual(tree.size(), (std::size_t)1000);
        util.assertSorted(tree.begin(), tree.end()); // std::less<> will compare first member in first, no duplicate first, so the default std::less<> is enough
        util.assertSorted(tree.begin(), tree.end(), cmpFirst);
        util.assertEqual(tree.empty(), false);

        // tree features
        {
            // construction
            IntTreap tree2(tree.begin(), tree.end(), std::less<int>(), std::less<int>());
            util.assertSorted(tree.begin(), tree.end());
            util.assertSequenceEqual(tree, tree2);
            // copy and move
            IntTreap tree3(tree2);
            util.assertSequenceEqual(tree2, tree3);
            IntTreap tree4(std::move(tree3));
            util.assertSequenceEqual(tree2, tree4);
            util.assertEqual(tree3.empty(), true);
            util.assertEqual(tree3.size(), (std::size_t)0);
            // copy and move assignment
            tree3 = tree2;
            util.assertSequenceEqual(tree2, tree3);
            tree3 = std::move(tree2);
            util.assertSequenceEqual(tree3, tree4);
            // size empty
            util.assertEqual(tree2.size(), (std::size_t)0);
            util.assertEqual(tree2.empty(), true);
            
            // iterators
            auto it = tree3.begin();
            util.assertEqual(getFirst(*it), 0);
            util.assertEqual(getFirst(*it++), 0);
            util.assertEqual(getFirst(*it), 1);
            util.assertEqual(getFirst(*++it), 2);
            util.assertEqual(getFirst(*it--), 2);
            util.assertEqual(getFirst(*--it), 0);

            // insert emplace
            auto p = std::make_pair(1024, 100);
            tree3.insert(p);
            tree3.insert(std::make_pair(1024, 78));
            tree3.emplace(1024, 103);
            util.assertEqual(tree3.size(), (std::size_t)1003);
            // erase find
            while (tree3.find(1024) != tree3.end())
            {
                tree3.erase(tree3.find(1024));
            }
            util.assertEqual(tree3.size(), (std::size_t)1000);
            util.assertSorted(tree3.begin(), tree3.end());
            // clear
            tree3.clear();
            util.assertEqual(tree3.size(), (std::size_t)0);
            util.assertEqual(tree3.empty(), true);
        }
        // heap features
        {
            // top pop
            std::vector<int> pri;
            while (!tree.empty())
            {
                pri.push_back(getSecond(tree.top()));
                tree.pop();
            }
            util.assertSorted(pri.begin(), pri.end(), std::greater<>());
            util.assertEqual(tree.size(), (std::size_t)0);
            util.assertEqual(tree.empty(), true);
        }
    }
}

void testTree(DetailFlag detail)
{
    TestUtil util(detail, "tree");
    testBinarySearchTree(util);
    testRedBlackTree(util);
    testTreap(util);
    util.showFinalResult();
}