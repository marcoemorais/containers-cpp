#include <cstddef>
#include <memory>
#include <stdexcept>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

namespace containers
{

struct deque_empty_error : std::runtime_error
{
    using std::runtime_error::runtime_error;
};

// node is a node in a double linked list.
template <typename T>
struct node
{
    node() = default;
    node(const T& data) : data(data) { }
    T data;
    std::unique_ptr<node<T>> next;
    node<T>* prev;
};

// Deque is a double ended queue that provides constant time front and back.
// You can think of the interface as the union of a Stack and Queue.
template <typename T>
class Deque
{
public:
    Deque() = default;

    // push_back appends data to end of deque.
    void push_back(const T& data)
    {
        if (!head) {
            head = std::make_unique<node<T>>(data);
            tail = head.get();
        }
        else {
            tail->next = std::make_unique<node<T>>(data);
            tail->next->prev = tail;
            tail = tail->next.get();
        }
        ++nelem;
    }

    // back returns but does not remove from back of deque.
    T& back()
    {
        if (!nelem) {
            throw deque_empty_error{"back from empty deuqe"};
        }
        return tail->data;
    }

    // pop_back removes but does not return from back of deque.
    void pop_back()
    {
        if (!nelem) {
            throw deque_empty_error{"pop_back from empty deque"};
        }
        if (head.get() == tail) {
            head = nullptr;
            tail = nullptr;
        }
        else {
            tail = tail->prev;
        }
        --nelem;
    }

    // push_front prepends data to the front of the deque.
    void push_front(const T& data)
    {
        if (!head) {
            head = std::make_unique<node<T>>(data);
            tail = head.get();
        }
        else {
            auto tmp = std::make_unique<node<T>>(data);
            tmp->next = std::move(head);
            head = std::move(tmp);
        }
        ++nelem;
    }

    // front returns but does not remove from front of deque.
    T& front()
    {
        if (!nelem) {
            throw deque_empty_error{"front from empty deque"};
        }
        return head->data;
    }

    // pop_front removes but does not return from front of deque.
    void pop_front()
    {
        if (!nelem) {
            throw deque_empty_error{"pop_front from empty deque"};
        }
        if (head.get() == tail) {
            head = nullptr;
            tail = nullptr;
        }
        else {
            head = std::move(head->next);
        }
        --nelem;
    }

    // size returns number of elements in deque.
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

TEST_CASE("[Deque]")
{
    using namespace containers;

    using T = int;
    Deque<T> deque;
    REQUIRE(deque.size() == 0);
    REQUIRE_THROWS_AS(deque.pop_back(), deque_empty_error);
    REQUIRE_THROWS_AS(deque.pop_front(), deque_empty_error);

    deque.push_front(1); // 1
    REQUIRE(deque.front() == 1);
    REQUIRE(deque.back() == 1);
    REQUIRE(deque.size() == 1);

    deque.push_front(2); // 2 1
    REQUIRE(deque.front() == 2);
    REQUIRE(deque.back() == 1);
    REQUIRE(deque.size() == 2);

    deque.push_back(3); // 2 1 3
    REQUIRE(deque.front() == 2);
    REQUIRE(deque.back() == 3);
    REQUIRE(deque.size() == 3);

    deque.push_back(4); // 2 1 3 4
    REQUIRE(deque.front() == 2);
    REQUIRE(deque.back() == 4);
    REQUIRE(deque.size() == 4);

    deque.pop_front(); // 1 3 4
    REQUIRE(deque.front() == 1);
    REQUIRE(deque.back() == 4);
    REQUIRE(deque.size() == 3);

    deque.pop_back(); // 1 3
    REQUIRE(deque.front() == 1);
    REQUIRE(deque.back() == 3);
    REQUIRE(deque.size() == 2);

    deque.pop_front(); // 3
    REQUIRE(deque.front() == 3);
    REQUIRE(deque.back() == 3);
    REQUIRE(deque.size() == 1);

    deque.pop_back(); // nil
    REQUIRE(deque.size() == 0);
    REQUIRE_THROWS_AS(deque.pop_back(), deque_empty_error);
    REQUIRE_THROWS_AS(deque.pop_front(), deque_empty_error);

    deque.push_front(1); // 1
    REQUIRE(deque.front() == 1);
    REQUIRE(deque.back() == 1);
    REQUIRE(deque.size() == 1);

    deque.pop_back(); // nil
    REQUIRE(deque.size() == 0);

    deque.push_back(2); // 2
    REQUIRE(deque.front() == 2);
    REQUIRE(deque.back() == 2);
    REQUIRE(deque.size() == 1);

    deque.pop_back(); // nil
    REQUIRE(deque.size() == 0);
}
