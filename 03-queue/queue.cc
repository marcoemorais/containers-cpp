#include <cstddef>
#include <forward_list>
#include <memory>
#include <stdexcept>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

namespace containers
{

struct queue_empty_error : std::runtime_error
{
    using std::runtime_error::runtime_error;
};

// Queue is a FIFO container of elements.
template <typename T>
class Queue
{
public:
    using List = std::forward_list<T>;
    using ListIterator = typename std::forward_list<T>::iterator;

    Queue() : tail(elems.end()) { }

    // push appends data to end of queue.
    void push(const T& data)
    {
        if (elems.empty()) {
            elems.push_front(data);
            tail = elems.begin();
        }
        else {
            tail = elems.insert_after(tail, data);
        }
        ++elems_size;
    }

    // pop removes but does not return from front of queue.
    void pop()
    {
        if (elems.empty()) {
            throw queue_empty_error{"pop from empty queue"};
        }
        elems.pop_front();
        if (elems.empty()) {
            tail = elems.end();
        }
        --elems_size;
    }

    // front returns but does not remove from front of queue.
    T& front()
    {
        if (elems.empty()) {
            throw queue_empty_error{"front from empty queue"};
        }
        return elems.front();
    }

    // size returns number of elements in queue.
    std::size_t size() const
    {
        return elems_size;
    }

private:
    List elems;
    ListIterator tail;
    std::size_t elems_size = 0;
};

}

TEST_CASE("[Queue]")
{
    using namespace containers;

    using T = int;
    Queue<T> queue;
    REQUIRE(queue.size() == 0);
    REQUIRE_THROWS_AS(queue.pop(), queue_empty_error);

    queue.push(1);
    REQUIRE(queue.front() == 1);
    REQUIRE(queue.size() == 1);

    queue.push(2);
    REQUIRE(queue.front() == 1);
    REQUIRE(queue.size() == 2);

    queue.push(3);
    REQUIRE(queue.front() == 1);
    REQUIRE(queue.size() == 3);

    queue.pop();
    REQUIRE(queue.front() == 2);
    REQUIRE(queue.size() == 2);

    queue.pop();
    REQUIRE(queue.front() == 3);
    REQUIRE(queue.size() == 1);

    queue.pop();
    REQUIRE(queue.size() == 0);
    REQUIRE_THROWS_AS(queue.front(), queue_empty_error);
}
