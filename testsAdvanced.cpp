#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <utility>
#include <string>
#include <chrono>
#include "catch.hpp"
#include "heap.h"

#define ALLOW_TESTS
#undef ALLOW_TESTS

#define MOVE_ONLY
#undef MOVE_ONLY

#ifdef ALLOW_TESTS

std::vector<int> getRange(int from, int to)
{
    std::vector<int> result;

    if(from < to)
    {
        for(int i = from; i <= to; ++i)
        {
            result.push_back(i);
        }
    }
    else
    {
        for(int i = from; i>=to; --i)
        {
            result.push_back(i);
        }
    }

    return result;
}

TEST_CASE("Ops with more data - MinHeap")
{
    MinHeap<int> heap;

    SECTION("Insert")
    {
        std::vector<int> vct;

        for(size_t i=100; i>0; --i)
        {
            vct.push_back(i);
        }

        for(auto a : vct)
        {
            heap.insert(a);
            REQUIRE(heap.top() == a);
        }

        auto srtHeap = toSortedVector(heap);
        REQUIRE(std::equal(srtHeap.begin(), srtHeap.end(), vct.rbegin(), vct.rend()));
    }

    SECTION("Update")
    {
        std::vector<int> vct;
        std::vector<MinHeap<int>::Handle> handles;

        for(size_t i=100; i>0; --i)
        {
            vct.push_back(i);
        }

        for(auto a : vct)
        {
            handles.push_back(heap.insert(a));
        }

        for(size_t i = 0; i < handles.size(); ++i)
        {
            if(i % 5 == 0)
            {
                heap.update(handles[i], heap.get(handles[i]) * -2);
            }
        }

        REQUIRE(heap.top() == -200);
        REQUIRE(handles[0] == heap.topHandle());

        heap.update(handles[10], -999);

        REQUIRE(heap.top() == -999);

        heap.update(handles[10], 1000);

        REQUIRE(heap.get(handles[10]) == 1000);

        while(heap.size() > 1)
        {
            heap.pop();
        }

        REQUIRE(heap.top() == 1000);
    }

    SECTION("Erase")
    {
        std::vector<int> vct;
        std::vector<MinHeap<int>::Handle> handles;

        for(size_t i=100; i>0; --i)
        {
            vct.push_back(i);
        }

        for(auto a : vct)
        {
            handles.push_back(heap.insert(a));
        }

        for(size_t i = 0; i < handles.size(); ++i)
        {
            if(i % 5 == 0)
            {
                heap.update(handles[i], heap.get(handles[i]) * -2);
            }
        }

        REQUIRE(heap.top() == -200);

        heap.erase(handles[0]);

        REQUIRE(heap.top() == -190);

        for(size_t i = 1; i<49; ++i)
            heap.erase(handles[i]);

        REQUIRE(heap.top() == -100);

        heap = MinHeap<int>({3,2,1});

        REQUIRE(heap.top() == 1);
        heap.erase(heap.topHandle());

        REQUIRE(heap.top() == 2);
        heap.update(heap.topHandle(), 4);

        REQUIRE(heap.top() == 3);
        heap.erase(heap.topHandle());

        REQUIRE(heap.top() == 4);
        heap.erase(heap.topHandle());

        REQUIRE(heap.empty());
        REQUIRE(heap.size() == 0);
    }

    SECTION("Combination")
    {
        for(auto a : getRange(100,0))
        {
            auto h = heap.insert(a);

            REQUIRE(heap.top() == a);

            heap.update(h, heap.get(h) * -1);
            auto h2 = heap.topHandle();

            REQUIRE(h == h2);

            heap.erase(h);
        }

        REQUIRE(heap.empty());

        for(auto a : {3, 5, 3})
            heap.insert(a);

        REQUIRE(heap.size() == 3);
        REQUIRE(heap.top() == 3);

        heap.pop();

        REQUIRE(heap.top() == 3);

        heap.update(heap.topHandle(), 6);

        REQUIRE(heap.size() == 2);
        REQUIRE(heap.top() == 5);

        heap.pop();

        REQUIRE(heap.size() == 1);
        REQUIRE(heap.top() == 6);

        heap.pop();

        REQUIRE(heap.empty());
    }
}

TEST_CASE("Ops with more data - MaxHeap")
{
    MaxHeap<int> heap;

    SECTION("Insert")
    {
        for(auto a : getRange(100,0))
        {
            heap.insert(a);
            REQUIRE(heap.top() == 100);
        }

        while(!heap.empty()) { heap.pop(); }

        for(auto a : getRange(0, 100))
        {
            heap.insert(a);
            REQUIRE(heap.top() == a);
        }
    }

    SECTION("Update")
    {
        std::vector<MaxHeap<int>::Handle> handles;

        for(auto a : getRange(0, 100))
        {
            handles.push_back(heap.insert(a));
        }

        for(auto h : handles)
        {
            int equal = 0;
            for(auto j : handles)
            {
                if(h==j) ++equal;
                if(h!=j) --equal;
            }
            REQUIRE(equal == -99);
        }

        for(auto j : handles)
        {
            heap.update(j, -heap.get(j));
        }

        REQUIRE(heap.top() == 0);

        heap.pop();

        REQUIRE(heap.top() == -1);
    }

    SECTION("Erase")
    {
        std::vector<MaxHeap<int>::Handle> handles;

        for(auto a : getRange(0, 100))
        {
            handles.push_back(heap.insert(a));
        }

        for(auto it = handles.rbegin(); it != handles.rend(); ++it)
        {
            auto val = heap.get(*it);
            heap.erase(*it);
            if(!heap.empty())
            {
                REQUIRE(heap.top() == val-1);
            }
        }
    }

    SECTION("Combination")
    {
        heap.erase(MaxHeap<int>::Handle());

        std::vector<MaxHeap<int>::Handle> handles;

        for(auto a : getRange(0, 100))
        {
            handles.push_back(heap.insert(a));
            REQUIRE(heap.top() == a);
        }

        for(auto h : handles)
        {
            if(heap.get(h) != 100)
            {
                heap.update(h, 999);
                REQUIRE(heap.top() == 999);
            }
        }

        for(auto h : handles)
        {
            if(heap.size() > 1)
            {
                heap.erase(h);
            }
        }

        REQUIRE(heap.size() == 1);
        REQUIRE(heap.top() == 100);

        REQUIRE(heap.get(handles[100]) == 100);

        heap.update(handles[100], 1);

        REQUIRE(heap.top() == 1);

        heap.insert(3);

        REQUIRE(heap.top() == 3);

        heap.pop();

        REQUIRE(heap.top() == 1);

        heap.pop();

        REQUIRE(heap.empty());
    }
}

TEST_CASE("Ops with more data - PriorityQueue")
{
    PriorityQueue<int, int> q;

    SECTION("Insert")
    {
        for(auto a : getRange(100,0))
        {
            q.insert({a, a});
        }

        std::vector<std::pair<int, int>> vct;
        for(auto a : getRange(100, 0))
            vct.push_back({a, a});

        REQUIRE(vct == toSortedVector(q));

        while(!q.empty()) { q.pop(); }

        vct.clear();

        for(auto a : getRange(0,100))
        {
            q.insert({a, a});
        }

        for(auto a : getRange(100, 0))
            vct.push_back({a, a});

        REQUIRE(vct == toSortedVector(q));
    }

    SECTION("Update")
    {
        std::vector<PriorityQueue<int, int>::Handle> handles;

        for(auto a : getRange(0,100))
        {
            handles.push_back(q.insert({a, a}));
        }

        for(auto h : handles)
        {
            q.update(h, {-q.get(h).first, -q.get(h).second});
        }

        REQUIRE(q.top() == std::make_pair(0,0));

        q.pop();

        REQUIRE(q.top() == std::make_pair(-1, -1));
    }

    SECTION("Erase")
    {
        std::vector<PriorityQueue<int, int>::Handle> handles;

        for(auto a : getRange(0,100))
        {
            handles.push_back(q.insert({a, a}));
        }

        for(auto h : handles)
        {
            q.update(h, {-q.get(h).first, -q.get(h).second});
        }

        for(auto h : handles)
        {
            if(q.get(h).first > -50)
                q.erase(h);
        }

        REQUIRE(q.top() == std::make_pair(-50, -50));

        q.insert({1, 1});

        REQUIRE(q.top() == std::make_pair(1,1));

        q.erase(q.topHandle());

        REQUIRE(q.top() == std::make_pair(-50, -50));
    }
}

TEST_CASE("Random data")
{
    srand(time(NULL));

    MinHeap<int> minheap;
    MaxHeap<int> maxheap;

    SECTION("1k data insert")
    {
        for(size_t i = 0; i < 100; ++i)
        {
            for(size_t i = 0; i < 1000; ++i)
            {
                int randNum = rand();

                minheap.insert(randNum);
                maxheap.insert(randNum);
            }

            std::vector<int> minvct = toSortedVector(minheap);
            std::vector<int> maxvct = toSortedVector(maxheap);

            REQUIRE(std::is_sorted(minvct.begin(), minvct.end()));
            REQUIRE(std::equal(minvct.begin(), minvct.end(), maxvct.rbegin(), maxvct.rend()));
        }
    }

    SECTION("1k data insert + erase")
    {
        for(size_t i = 0; i < 100; ++i)
        {
            for(size_t i = 0; i < 1000; ++i)
            {
                int randNum = rand();

                minheap.insert(randNum);
                maxheap.insert(randNum);
            }

            for(int i = 0; i < rand() % 100; ++i)
            {
                minheap.pop();
                maxheap.pop();
            }

            std::vector<int> minvct = toSortedVector(minheap);
            std::vector<int> maxvct = toSortedVector(maxheap);

            REQUIRE(std::is_sorted(minvct.begin(), minvct.end()));
            REQUIRE(std::is_sorted(maxvct.rbegin(), maxvct.rend()));
        }
    }

    SECTION("1k data erase random")
    {
        for(size_t cnt = 0; cnt < 20; ++cnt)
        {
            std::vector<MinHeap<int>::Handle> minHandles;
            std::vector<MaxHeap<int>::Handle> maxHandles;

            for(size_t i = 0; i < 1000; ++i)
            {
                int randNum = rand();
                minHandles.push_back(minheap.insert(randNum));
                maxHandles.push_back(maxheap.insert(randNum));
            }

            for(int i = 0; i < 256; ++i)
            {
                int rnd = rand();
                minheap.erase(minHandles[rnd % (minHandles.size() - 1)]);
                minHandles.erase(minHandles.begin()+ rnd % (minHandles.size() - 1));
                maxheap.erase(maxHandles[rnd % (maxHandles.size() - 1)]);
                maxHandles.erase(maxHandles.begin()+rnd % (maxHandles.size() - 1));
            }

            std::vector<int> minvct = toSortedVector(minheap);
            std::vector<int> maxvct = toSortedVector(maxheap);

            REQUIRE(minvct.size() == maxvct.size());
            REQUIRE(minvct.size() == 744);
            REQUIRE(std::is_sorted(minvct.begin(), minvct.end()));
            REQUIRE(std::is_sorted(maxvct.rbegin(), maxvct.rend()));

            while(!minheap.empty()) { minheap.pop(); }
            while(!maxheap.empty()) { maxheap.pop(); }
        }
    }
}

#endif

#ifdef MOVE_ONLY

struct MoveOnly
{
    MoveOnly() = delete;

    MoveOnly(int x)
        :mX(x) { }

    MoveOnly(MoveOnly&&) noexcept = default;

    MoveOnly(const MoveOnly &) = delete;

    MoveOnly& operator=(MoveOnly&&) noexcept = default;

    MoveOnly& operator=(const MoveOnly&) = delete;


    friend struct MoveOnlyCmp;
    friend bool operator==(const MoveOnly&, const MoveOnly&);

    ~MoveOnly() noexcept = default;

private:
    int mX;
};

bool operator==(const MoveOnly& lhs, const MoveOnly& rhs)
{
    return lhs.mX == rhs.mX;
}

bool operator!=(const MoveOnly& lhs, const MoveOnly& rhs)
{
    return !(lhs==rhs);
}

struct MoveOnlyCmp
{
    bool operator()(const MoveOnly& lhs, const MoveOnly& rhs)
    {
        return lhs.mX > rhs.mX;
    }
};

TEST_CASE("MoveOnly tests")
{
    Heap<MoveOnly, MoveOnlyCmp> heap;

    SECTION("insert")
    {
        heap.insert(MoveOnly(3));

        REQUIRE(heap.top() == MoveOnly(3));

        MoveOnly newMove(4);

        heap.insert(std::move(newMove));

        REQUIRE(heap.top() == MoveOnly(3));

        heap.pop();

        REQUIRE(heap.top() == MoveOnly(4));

        heap.pop();

        REQUIRE(heap.empty());

        for(int i = 0; i < 100; ++i)
        {
            heap.insert(MoveOnly(i));
        }

        REQUIRE(heap.size() == 100);
    }

    SECTION("update")
    {
        auto h = heap.insert(MoveOnly(3));

        heap.insert(MoveOnly(4));

        REQUIRE(heap.top() == MoveOnly(3));

        heap.update(h, MoveOnly(5));

        REQUIRE(heap.top() == MoveOnly(4));

        heap.pop();

        REQUIRE(heap.top() == MoveOnly(5));
    }
}
#endif
