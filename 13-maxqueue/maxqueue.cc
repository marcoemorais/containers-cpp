#include <cstddef>
#include <deque>
#include <queue>
#include <stdexcept>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#if __has_include("containers/print.h")
#   include "containers/print.h"
#endif

namespace containers
{

struct queue_empty_error : std::runtime_error
{
    using std::runtime_error::runtime_error;
};

// MaxQueue is a FIFO container with constant time access to maximum value.
template <typename T>
class MaxQueue
{
public:
    MaxQueue() = default;

    // push adds element to end of queue.
    void push(const T& v)
    {
        elems.push(v);
        while (!maxv.empty() && v > maxv.back()) {
            // Remove all values smaller than max.
            maxv.pop_back();
        }
        maxv.push_back(v);
    }

    // front returns but does not remove front of queue.
    const T& front() const
    {
        if (elems.empty()) {
            throw queue_empty_error{"front from empty queue"};
        }
        return elems.front();
    }

    // max returns but does not remove maximum value in queue.
    const T& max() const
    {
        if (elems.empty()) {
            throw queue_empty_error{"max from empty queue"};
        }
        return maxv.front();
    }

    // pop removes but does not return front of queue.
    void pop()
    {
        if (elems.empty()) {
            throw queue_empty_error{"pop from empty queue"};
        }
        if (elems.front() == maxv.front()) {
            maxv.pop_front();
        }
        elems.pop();
    }

    // size returns number of elements in queue.
    std::size_t size() const
    {
        return elems.size();
    }

private:
    std::queue<T> elems;
    std::deque<T> maxv;
};

}

TEST_CASE("[MaxQueue]")
{
    using namespace containers;

    using T = int;
    MaxQueue<T> queue;
    REQUIRE(queue.size() == 0);
    REQUIRE_THROWS_AS(queue.front(), queue_empty_error);
    REQUIRE_THROWS_AS(queue.max(), queue_empty_error);
    REQUIRE_THROWS_AS(queue.pop(), queue_empty_error);

    // 1-3-2
    queue.push(1);
    REQUIRE(queue.front() == 1);
    REQUIRE(queue.max() == 1);
    REQUIRE(queue.size() == 1);

    queue.push(3);
    REQUIRE(queue.front() == 1);
    REQUIRE(queue.max() == 3);
    REQUIRE(queue.size() == 2);

    queue.push(2);
    REQUIRE(queue.front() == 1);
    REQUIRE(queue.max() == 3);
    REQUIRE(queue.size() == 3);

    queue.pop();
    REQUIRE(queue.front() == 3);
    REQUIRE(queue.max() == 3);
    REQUIRE(queue.size() == 2);

    queue.pop();
    REQUIRE(queue.front() == 2);
    REQUIRE(queue.max() == 2);
    REQUIRE(queue.size() == 1);

    queue.pop();
    REQUIRE(queue.size() == 0);
    REQUIRE_THROWS_AS(queue.front(), queue_empty_error);
    REQUIRE_THROWS_AS(queue.max(), queue_empty_error);

    // 1-2-3
    queue.push(1);
    REQUIRE(queue.front() == 1);
    REQUIRE(queue.max() == 1);
    REQUIRE(queue.size() == 1);

    queue.push(2);
    REQUIRE(queue.front() == 1);
    REQUIRE(queue.max() == 2);
    REQUIRE(queue.size() == 2);

    queue.push(3);
    REQUIRE(queue.front() == 1);
    REQUIRE(queue.max() == 3);
    REQUIRE(queue.size() == 3);

    queue.pop();
    REQUIRE(queue.front() == 2);
    REQUIRE(queue.max() == 3);
    REQUIRE(queue.size() == 2);

    queue.pop();
    REQUIRE(queue.front() == 3);
    REQUIRE(queue.max() == 3);
    REQUIRE(queue.size() == 1);

    queue.pop();
    REQUIRE(queue.size() == 0);
    REQUIRE_THROWS_AS(queue.front(), queue_empty_error);
    REQUIRE_THROWS_AS(queue.max(), queue_empty_error);

    // 3-2-1
    queue.push(3);
    REQUIRE(queue.front() == 3);
    REQUIRE(queue.max() == 3);
    REQUIRE(queue.size() == 1);

    queue.push(2);
    REQUIRE(queue.front() == 3);
    REQUIRE(queue.max() == 3);
    REQUIRE(queue.size() == 2);

    queue.push(1);
    REQUIRE(queue.front() == 3);
    REQUIRE(queue.max() == 3);
    REQUIRE(queue.size() == 3);

    queue.pop();
    REQUIRE(queue.front() == 2);
    REQUIRE(queue.max() == 2);
    REQUIRE(queue.size() == 2);

    queue.pop();
    REQUIRE(queue.front() == 1);
    REQUIRE(queue.max() == 1);
    REQUIRE(queue.size() == 1);

    queue.pop();
    REQUIRE(queue.size() == 0);
    REQUIRE_THROWS_AS(queue.front(), queue_empty_error);
    REQUIRE_THROWS_AS(queue.max(), queue_empty_error);
}
