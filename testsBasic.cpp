#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <utility>
#include <chrono>
#include "catch.hpp"
#include "heap.h"

#define ALLOW_TESTS
#undef ALLOW_TESTS

#ifdef ALLOW_TESTS

TEST_CASE("Constructors")
{
    SECTION("Default constructor")
    {
        MinHeap<int> h;
        REQUIRE(h.empty());
        REQUIRE(h.size() == 0);
    }

    SECTION("Copy constructor")
    {
        MinHeap<int> h;

        SECTION("Insert value pretest")
        {
            h.insert(1);
            REQUIRE_FALSE(h.empty());
            REQUIRE(h.size() == 1);
            REQUIRE(h.top() == 1);
        }

        MinHeap<int> h2 = h;
        REQUIRE_FALSE(h2.empty());
        REQUIRE(h2.size() == 1);
        REQUIRE(h2.top() == 1);
    }

    SECTION("Move constructor")
    {
        MinHeap<int> h;

        SECTION("Insert value pretest")
        {
            h.insert(2);
            REQUIRE_FALSE(h.empty());
            REQUIRE(h.size() == 1);
            REQUIRE(h.top() == 2);
        }

        MinHeap<int> h2 = std::move(h);
        REQUIRE_FALSE(h2.empty());
        REQUIRE(h2.size() == 1);
        REQUIRE(h2.top() == 2);
    }

    SECTION("Iterator constructor")
    {
        std::vector<int> vct = { 3, 5, 2, 5 };
        MinHeap<int> h(vct.begin(), vct.end());

        REQUIRE(h.size() == 4);
        REQUIRE(h.top() == 2);
    }

    SECTION("Initializer list constructor")
    {
        MinHeap<int> h = {3, 5, 2, 5};

        REQUIRE(h.size() == 4);
        REQUIRE(h.top() == 2);
    }
}

TEST_CASE("Basic operations")
{
    MinHeap<int> h;

    SECTION("Empty heap")
    {
        h.pop();
        h.update(MinHeap<int>::Handle(),3);
        h.erase(MinHeap<int>::Handle());

        REQUIRE(h.empty());
        REQUIRE(h.size() == 0);
    }

    SECTION("Insert & erase")
    {
        for(int i=10; i>=-10; --i)
        {
            auto handle = h.insert(i);

            REQUIRE(h.top() == i);
            REQUIRE(h.topHandle() == handle);

            h.erase(handle);
        }
    }

    SECTION("Insert & update & erase")
    {
        std::vector<MinHeap<int>::Handle> handles;

        for(int i=20; i>=0; --i)
        {
            handles.push_back(h.insert(i));
        }

        for(int i=0; i<=20; ++i)
        {
            REQUIRE((20-i) == h.get(handles[i]));
            h.update(handles[i], i);
        }

        REQUIRE(h.topHandle() == handles[0]);

        for(int i=0; i<=20; ++i)
        {
            REQUIRE(i == h.get(handles[i]));
        }

        REQUIRE(h.size() == handles.size());
        REQUIRE(h.topHandle() == handles[0]);
        REQUIRE(h.top() == 0);

        for(int i=0; i<=20; ++i)
        {
            h.erase(handles[i]);
        }

        REQUIRE(h.size() == 0);
        REQUIRE(h.empty());
    }

    SECTION("Sequential pop")
    {
        h = MinHeap<int>({7, 4, 2, 9, 15, 3, 6});

        REQUIRE(h.get(h.topHandle()) == h.top());
        REQUIRE(h.top() == 2);
        h.pop();

        REQUIRE(h.get(h.topHandle()) == h.top());
        REQUIRE(h.top() == 3);
        h.pop();

        REQUIRE(h.get(h.topHandle()) == h.top());
        REQUIRE(h.top() == 4);
        h.pop();

        REQUIRE(h.get(h.topHandle()) == h.top());
        REQUIRE(h.top() == 6);
        h.pop();

        REQUIRE(h.get(h.topHandle()) == h.top());
        REQUIRE(h.top() == 7);
        h.pop();

        REQUIRE(h.get(h.topHandle()) == h.top());
        REQUIRE(h.top() == 9);
        h.pop();

        REQUIRE(h.get(h.topHandle()) == h.top());
        REQUIRE(h.top() == 15);
        h.pop();

        REQUIRE(h.size() == 0);
        REQUIRE(h.empty());
    }

    SECTION("toSorted Vector")
    {
        std::vector<int> vct {3, 7, 8, 1, 2, 9, 15, 7, 9, 2, -4, 78, 43};
        h = MinHeap<int>(vct.begin(), vct.end());

        std::sort(vct.begin(), vct.end());

        REQUIRE(vct == toSortedVector(h));
    }
}

TEST_CASE("Handles stuff")
{
    MinHeap<int> h {4, 7, 1, 3, 9};

    SECTION("Move constructor - handle validity")
    {
        auto handle = h.topHandle();

        REQUIRE(handle == h.topHandle());
        REQUIRE(h.get(handle) == 1);

        auto h2 = std::move(h);

        REQUIRE(handle == h2.topHandle());
        REQUIRE(h2.get(handle) == 1);

        h2.pop();

        REQUIRE(h2.top() == 3);
    }

    SECTION("After-swap handle validity")
    {
        MinHeap<int> h2 {3, 5, 2};

        auto handle1 = h.topHandle();
        auto handle2 = h2.insert(4);

        REQUIRE(h.get(handle1) == 1);
        REQUIRE(h2.get(handle2) == 4);
        REQUIRE(h.size() == 5);
        REQUIRE(h2.size() == 4);

        h.swap(h2);

        REQUIRE(h.get(handle2) == 4);
        REQUIRE(h2.get(handle1) == 1);
        REQUIRE(handle1 == h2.topHandle());
        REQUIRE(h.size() == 4);
        REQUIRE(h2.size() == 5);
    }
}

class CopyFailer
{
private:
    int mX;
public:
    CopyFailer() = delete;

    CopyFailer(int x):mX(x) { }

    CopyFailer(const CopyFailer&):mX(-1) { FAIL("Do not call copy ctor ... suka blyat!"); }

    CopyFailer& operator=(const CopyFailer&)
    {
        FAIL("Do not call copy op= ... suka blyat!");
        return *this;
    }

    CopyFailer(CopyFailer&&) = default;

    CopyFailer& operator=(CopyFailer&&) = default;

    int getValue() const
    {
        return mX;
    }
};

struct CopyFailerCmp
{
    bool operator()(const CopyFailer& x, const CopyFailer& y) const
    {
        return x.getValue() > y.getValue();
    }
};

CopyFailer generateCopyFailer()
{
    return CopyFailer(3);
}

TEST_CASE("Move semantics")
{
    Heap<CopyFailer, CopyFailerCmp> h;

    SECTION("Insert")
    {
        h.insert(CopyFailer(1));

        REQUIRE(h.size() == 1);
        REQUIRE(h.top().getValue() == 1);

        CopyFailer cp = generateCopyFailer();
        h.insert(std::move(cp));

        REQUIRE(h.size() == 2);
        REQUIRE(h.top().getValue() == 1);
        h.pop();
        REQUIRE(h.top().getValue() == 3);
    }

    SECTION("Update")
    {
        auto handle = h.insert(CopyFailer(1));
        h.update(handle, CopyFailer(2));

        REQUIRE(h.size() == 1);
        REQUIRE(h.top().getValue() == 2);

        CopyFailer cp = generateCopyFailer();
        h.insert(std::move(cp));

        REQUIRE(h.size() == 2);
        REQUIRE(h.top().getValue() == 2);
        h.pop();
        REQUIRE(h.top().getValue() == 3);
    }
}

TEST_CASE("Heap creation")
{
    std::vector<int> vct;
    MinHeap<int> h;

    SECTION("Sorted data - insert")
    {
        for(size_t i = 0; i < 100; ++i)
        {
            vct.push_back(i);
        }

        for(auto a : vct)
            h.insert(a);

        REQUIRE(vct == toSortedVector(h));
    }

    SECTION("Sorted data - ctor")
    {
        for(size_t i = 0; i < 100; ++i)
        {
            vct.push_back(i);
        }

        h = MinHeap<int>(vct.begin(), vct.end());

        REQUIRE(vct == toSortedVector(h));
    }


    SECTION("Reversed data - insert")
    {
        for(size_t i = 100; i > 0; --i)
        {
            vct.push_back(i);
        }

        for(auto a : vct)
            h.insert(a);

        std::sort(vct.begin(), vct.end());

        REQUIRE(vct == toSortedVector(h));
    }

    SECTION("Reversed data - ctor")
    {
        for(size_t i = 100; i > 0; --i)
        {
            vct.push_back(i);
        }

        h = MinHeap<int>(vct.begin(), vct.end());

        std::sort(vct.begin(), vct.end());

        REQUIRE(vct == toSortedVector(h));
    }

    SECTION("Update data")
    {
        srand(time(NULL));

        std::vector<MinHeap<int>::Handle> handles;

        for(size_t i = 100; i > 0; --i)
        {
            vct.push_back(i);
        }

        for(auto a : vct) { handles.push_back(h.insert(a)); }

        std::sort(vct.begin(), vct.end());
        REQUIRE(vct == toSortedVector(h));

        for(auto hd : handles) { h.update(hd, rand()%70); }

        std::vector<int> vct2 = toSortedVector(h);
        REQUIRE(std::is_sorted(vct2.begin(), vct2.end()));
    }
}

template<typename Func, typename Data>
auto getDurationTime(Func f, const Data& c)
{
    using namespace std;
    using namespace std::chrono;

    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    f(c);
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    return duration_cast<microseconds>( t2 - t1 ).count();
}

void hugeInsertion(const std::vector<int>& vct)
{
    MinHeap<int> h;

    for(auto a : vct)
        h.insert(a);

    std::vector<int> srt = toSortedVector(h);
    REQUIRE(std::is_sorted(srt.begin(), srt.end()));
}

void hugeCtor(const std::vector<int>& vct)
{
    MinHeap<int> h(vct.begin(), vct.end());

    std::vector<int> srt = toSortedVector(h);
    REQUIRE(std::is_sorted(srt.begin(), srt.end()));
}

void updateFunction(const std::vector<int>& vctDef)
{
    MinHeap<int> h;
    std::vector<MinHeap<int>::Handle> handles;
    std::vector<int> vct = vctDef;

    for(auto a : vct)
        handles.push_back(h.insert(a));

    for(size_t i = 0; i < vct.size(); ++i)
        vct[i] *= -1;

    for(size_t i = 0; i < handles.size(); ++i)
        h.update(handles[i], vct[i]);

    std::sort(vct.begin(), vct.end());
    REQUIRE(vct == toSortedVector(h));

}

void insEraseFunction(const std::vector<int>& vctDef)
{
    MinHeap<int> h;
    std::vector<MinHeap<int>::Handle> handles;
    std::vector<int> vct = vctDef;

    for(auto a : vct)
        handles.push_back(h.insert(a));

    for(size_t i = 0; i < vct.size(); ++i)
        vct[i] *= -1;

    for(size_t i = 0; i < handles.size(); ++i)
        h.erase(handles[i]);

    for(auto a : vct)
        h.insert(a);

    std::sort(vct.begin(), vct.end());
    REQUIRE(vct == toSortedVector(h));

}

TEST_CASE("Time measure")
{
    SECTION("Insert vs ctor")
    {
        for(size_t cnt = 523; cnt<1024; cnt+=100)
        {
            std::vector<int> vct;
            for(size_t i = cnt * cnt; i>0; --i)
            {
                vct.push_back(i);
            }

            auto insTime = getDurationTime(hugeInsertion, vct);
            auto ctorTime = getDurationTime(hugeCtor, vct);

            REQUIRE(ctorTime < insTime);
        }
    }
}

TEST_CASE("Priority queue")
{
    PriorityQueue<int, std::string> q;
    std::vector<std::pair<int, std::string>> vct;

    for(size_t i = 0; i<30; ++i)
    {
        auto item = std::make_pair(i, "secondElement");
        q.insert(item);
        vct.push_back(item);
    }

    auto qVct = toSortedVector(q);
    REQUIRE(std::equal(qVct.begin(), qVct.end(), vct.rbegin(), vct.rend()));
}

#endif
