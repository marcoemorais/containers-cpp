#include <cstddef>
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

// node is a node in the queue.
template <typename T>
struct node
{
    node() = default;
    node(const T& data) : data(data) { }
    T data;
    std::unique_ptr<node<T>> next;
};

// Queue is a FIFO container of elements.
template <typename T>
class Queue
{
public:
    Queue() = default;

    // push appends data to end of queue.
    void push(const T& data)
    {
        if (!head) {
            // Queue is empty.
            head = std::make_unique<node<T>>(data);
            tail = head.get();
        }
        else {
            tail->next = std::make_unique<node<T>>(data);
            tail = tail->next.get();
        }
        ++nelem;
    }

    // pop removes but does not return from front of queue.
    void pop()
    {
        if (!head) {
            throw queue_empty_error{"pop from empty queue"};
        }
        head = std::move(head->next);
        --nelem;
    }

    // front returns but does not remove from front of queue.
    T& front()
    {
        if (!head) {
            throw queue_empty_error{"front from empty queue"};
        }
        return head->data;
    }

    // size returns number of elements in queue.
    std::size_t size() const
    {
        return nelem;
    }

private:
    std::unique_ptr<node<T>> head;
    node<T>* tail;
    std::size_t nelem = 0;
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
