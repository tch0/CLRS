#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <source_location> // since C++20
#include <algorithm>
#include <iterator>
#include <utility>
#include <tuple>

enum class DetailFlag
{
    ShowNothing,        // show final result only, basic output
    ShowFailedOnly,     // show failed results only
    ShowAllResults      // show passed and failed results
};

// parsing first argument: -d to show details
inline DetailFlag parseDetailFlag(int argc, char const *argv[])
{
    if (argc >= 2 && std::string(argv[1]) == "-all")
    {
        return DetailFlag::ShowAllResults;
    }
    else if (argc >= 2 && std::string(argv[1]) == "-fo")
    {
        return DetailFlag::ShowFailedOnly;
    }
    else
    {
        return DetailFlag::ShowNothing;
    }
}

// operator << for pair
template<typename T1, typename T2>
std::ostream& operator<<(std::ostream& os, const std::pair<T1, T2>& p)
{
    os << "(" << p.first << ", " << p.second << ")";
    return os;
}

// operator << for tuple
template<std::size_t Index, typename... Args>
std::ostream& printTuple(std::ostream& os, const std::tuple<Args...>& t)
{
    if constexpr (Index == 0) // first element
    {
        os << "(";
    }
    if constexpr (Index < sizeof...(Args) - 1)
    {
        os << std::get<Index>(t) << ", ";
        return printTuple<Index + 1, Args...>(os, t);
    }
    else // last element
    {
        return os << std::get<Index>(t) << ")";
    }
}
template<typename... Args>
std::ostream& operator<<(std::ostream& os, const std::tuple<Args...>& t)
{
    return printTuple<0, Args...>(os, t);
}

// manipulator for printing first N elements of a sequence
template<typename Iterator>
class PrintSequenceElements
{
    friend std::ostream& operator<<(std::ostream& os, const PrintSequenceElements& p)
    {
        std::size_t count = 0;
        auto iter = p.begin;
        for (; iter != p.end && count < p.num; ++count, ++iter)
        {
            os << *iter << " ";
        }
        if (iter != p.end)
        {
            os << "...";
        }
        return os;
    }
public:
    PrintSequenceElements(const Iterator& _begin, const Iterator& _end, std::size_t _num) : begin(_begin), end(_end), num(_num)
    {
    }
private:
    const Iterator begin;
    const Iterator end;
    std::size_t num;
};

template<typename Container>
PrintSequenceElements<typename Container::const_iterator> printContainerElememts(const Container& c, std::size_t num)
{
    return PrintSequenceElements<typename Container::const_iterator>(c.begin(), c.end(), num);
}

template<typename T>
PrintSequenceElements<T*> printArrayElements(T* arr, std::size_t size, std::size_t num)
{
    return PrintSequenceElements<T*>(arr, arr+size, num);
}

// test utilities
class TestUtil
{
private:
    bool showDetails(bool result)
    {
        return (detail == DetailFlag::ShowFailedOnly && !result) || detail == DetailFlag::ShowAllResults;
    }
public:
    TestUtil(DetailFlag _detail, const std::string& _target, int _targetAlignWidth = 20, int _lineNumberAlignWidth = 4, int _maxSequenceLength = 20, int _countAlignWidth = 3)
        : passedCount(0)
        , totalCount(0)
        , detail(_detail)
        , target(_target)
        , targetAlignWidth(_targetAlignWidth)
        , lineNumberAlignWidth(_lineNumberAlignWidth)
        , maxSequenceLength(_maxSequenceLength)
        , countAlignWidth(_countAlignWidth)
    {
        if (detail != DetailFlag::ShowNothing)
        {
            std::cout << "Test result of " << target << ": " << std::endl;
        }
    }

    void showFinalResult()
    {
        std::cout << std::boolalpha << std::dec;
        std::cout << std::setw(targetAlignWidth) << std::right << target << " : "
            << std::setw(countAlignWidth) << std::right << passedCount << " / " << std::setw(countAlignWidth) << std::left << totalCount << " passed"
            << (passedCount == totalCount ? "" : " --------------------------> failed") << std::endl;
        if (detail != DetailFlag::ShowNothing)
        {
            std::cout << std::endl;
        }
    }

    template<typename T1, typename T2>
    void assertEqual(const T1& t1, const T2& t2, const std::source_location& loc = std::source_location::current())
    {
        bool res = (t1 == t2);
        passedCount += (res ? 1 : 0);
        totalCount++;
        if (showDetails(res))
        {
            std::cout << std::boolalpha << std::dec;
            std::cout << loc.file_name() << ":" << std::setw(lineNumberAlignWidth) << loc.line() << ": "
                << "assertEqual: " << "left value( " << t1 << " ), right value( " << t2 << " ) : "
                << (res ? "passed" : "==================== failed") << std::endl;
        }
    }

    template<typename T1, typename T2>
    void assertNotEqual(const T1& t1, const T2& t2, const std::source_location& loc = std::source_location::current())
    {
        bool res = (t1 != t2);
        passedCount += (res ? 1 : 0);
        totalCount++;
        if (showDetails(res))
        {
            std::cout << std::boolalpha << std::dec;
            std::cout << loc.file_name() << ":" << std::setw(lineNumberAlignWidth) << loc.line() << ": "
                << "assertNotEqual: " << "left value( " << t1 << " ), right value( " << t2 << " ) : "
                << (res ? "passed" : "==================== failed") << std::endl;
        }
    }

    template<typename Container1, typename Container2>
    void assertSequenceEqual(const Container1& c1, const Container2& c2, const std::source_location& loc = std::source_location::current())
    {
        bool res = std::equal(c1.begin(), c1.end(), c2.begin());
        passedCount += (res ? 1 : 0);
        totalCount++;
        if (showDetails(res))
        {
            std::cout << std::boolalpha << std::dec;
            std::cout << loc.file_name() << ":" << std::setw(lineNumberAlignWidth) << loc.line() << ": "
                << "assertSequenceEqual: " << (res ? "passed" : "==================== failed")
                << "\n\tleft sequence: " << printContainerElememts(c1, maxSequenceLength)
                << "\n\tright sequence: " << printContainerElememts(c2, maxSequenceLength) << std::endl;
        }
    }

    template<typename T1, typename T2>
    void assertArrayEqual(const T1* arr1, const T2* arr2, std::size_t size, const std::source_location& loc = std::source_location::current())
    {
        bool res = std::equal(arr1, arr1 + size, arr2);
        passedCount += (res ? 1 : 0);
        totalCount++;
        if (showDetails(res))
        {
            std::cout << std::boolalpha << std::dec;
            std::cout << loc.file_name() << ":" << std::setw(lineNumberAlignWidth) << loc.line() << ": "
                << "assertArrayEqual: " << (res ? "passed" : "==================== failed")
                << "\n\tleft array: " << printArrayElements(arr1, size, maxSequenceLength)
                << "\n\tright array: " << printArrayElements(arr2, size, maxSequenceLength) << std::endl;
        }
    }

    // more generic version of assert sequence/array equal
    template<typename ForwardIterator1, typename ForwardIterator2>
    void assertRangeEqual(ForwardIterator1 b1, ForwardIterator1 e1, ForwardIterator2 b2, const std::source_location& loc = std::source_location::current())
    {
        bool res = std::equal(b1, e1, b2);
        passedCount += (res ? 1 : 0);
        totalCount++;
        if (showDetails(res))
        {
            std::cout << std::boolalpha << std::dec;
            std::cout << loc.file_name() << ":" << std::setw(lineNumberAlignWidth) << loc.line() << ": "
                << "assertRangeEqual: " << (res ? "passed" : "==================== failed")
                << "\n\tleft range: " << PrintSequenceElements(b1, e1, maxSequenceLength)
                << "\n\tright range: " << PrintSequenceElements(b2, std::next(b2, std::distance(b1, e1)), maxSequenceLength)  << std::endl;
        }
    }
    template<typename ForwardIterator1, typename ForwardIterator2>
    void assertRangeEqual(ForwardIterator1 b1, ForwardIterator1 e1, ForwardIterator2 b2, ForwardIterator2 e2, const std::source_location& loc = std::source_location::current())
    {
        bool res = std::distance(b1, e1) == std::distance(b2, e2) && std::equal(b1, e1, b2);
        passedCount += (res ? 1 : 0);
        totalCount++;
        if (showDetails(res))
        {
            std::cout << std::boolalpha << std::dec;
            std::cout << loc.file_name() << ":" << std::setw(lineNumberAlignWidth) << loc.line() << ": "
                << "assertRangeEqual: " << (res ? "passed" : "==================== failed")
                << "\n\tleft range: " << PrintSequenceElements(b1, e1, maxSequenceLength)
                << "\n\tright range: " << PrintSequenceElements(b2, e2, maxSequenceLength)  << std::endl;
        }
    }
    // assert a sequence is sorted
    template<typename InputIterator, typename Compare = std::less<typename std::iterator_traits<InputIterator>::value_type>>
    void assertSorted(InputIterator b, InputIterator e, const Compare& cmp = Compare(), const std::source_location& loc = std::source_location::current())
    {
        bool res = std::is_sorted(b, e, cmp);
        passedCount += (res ? 1 : 0);
        totalCount++;
        if (showDetails(res))
        {
            std::cout << std::boolalpha << std::dec;
            std::cout << loc.file_name() << ":" << std::setw(lineNumberAlignWidth) << loc.line() << ": "
                << "assertSorted: " << (res ? "passed" : "==================== failed")
                << "\n\tsequence: " << PrintSequenceElements(b, e, maxSequenceLength) << std::endl;
        }
    }
    // assert two set is equal, do not consider order of elements.
    template<typename Container1, typename Container2>
    void assertSetEqual(const Container1& c1, const Container2& c2, const std::source_location& loc = std::source_location::current())
    {
        bool res = (std::size(c1) == std::size(c2) && std::is_permutation(c1.begin(), c1.end(), c2.begin()));
        passedCount += (res ? 1 : 0);
        totalCount++;
        if (showDetails(res))
        {
            std::cout << std::boolalpha << std::dec;
            std::cout << loc.file_name() << ":" << std::setw(lineNumberAlignWidth) << loc.line() << ": "
                << "assertSetEqual: " << (res ? "passed" : "==================== failed")
                << "\n\tleft set: " << printContainerElememts(c1, maxSequenceLength)
                << "\n\tright set: " << printContainerElememts(c2, maxSequenceLength) << std::endl;
        }
    }
    template<typename ForwardIterator1, typename ForwardIterator2>
    void assertSetEqual(ForwardIterator1 b1, ForwardIterator1 e1, ForwardIterator2 b2, ForwardIterator2 e2, const std::source_location& loc = std::source_location::current())
    {
        bool res = (std::distance(b1, e1) == std::distance(b2, e2) && std::is_permutation(b1, e1, b2));
        passedCount += (res ? 1 : 0);
        totalCount++;
        if (showDetails(res))
        {
            std::cout << std::boolalpha << std::dec;
            std::cout << loc.file_name() << ":" << std::setw(lineNumberAlignWidth) << loc.line() << ": "
                << "assertSetEqual: " << (res ? "passed" : "==================== failed")
                << "\n\tleft set: " << PrintSequenceElements(b1, e1, maxSequenceLength)
                << "\n\tright set: " << PrintSequenceElements(b2, e2, maxSequenceLength)  << std::endl;
        }
    }
private:
    int passedCount;
    int totalCount;
    DetailFlag detail;
    std::string target;
    int targetAlignWidth;       // align width of target string
    int lineNumberAlignWidth;   // align width of line number
    int maxSequenceLength;      // max output length of a sequence
    int countAlignWidth;        // align width of count number
};
