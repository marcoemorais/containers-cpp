#include <algorithm>
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <vector>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#if __has_include("containers/print.h")
#   include "containers/print.h"
#endif

namespace containers
{

struct heap_empty_error : std::runtime_error
{
    using std::runtime_error::runtime_error;
};

// Heap provides constant time access to min or max value.
// Inserting elements into the heap requires lg2(N) time.
template <typename T,
          typename Compare = std::less<T>>
class Heap
{
public:
    Heap() = default;

    // push adds value into the heap.
    void push(const T& v)
    {
        tree.push_back(v);
        bubbleUp(tree.size()-1);
    }

    // top returns but does not remove from top of heap.
    T& top()
    {
        if (size() < 1) {
            throw heap_empty_error{"top from empty heap"};
        }
        return tree[1];
    }

    // pop removes but does not return from top of heap.
    void pop()
    {
        if (size() < 1) {
            throw heap_empty_error{"pop from empty heap"};
        }
        // Move rightmost child to root.
        std::swap(tree[1], tree[tree.size()-1]);
        tree.resize(tree.size()-1);
        if (size() > 1) {
            bubbleDown(1);
        }
    }

    // size returns the number of elements in the heap.
    std::size_t size() const
    {
        return tree.size()-1;
    }

private:
    // Use a vector indexed from 1 to simplify index computations.
    std::vector<T> tree{T()};

    Compare comp_fn; // Comparison function.

    // bubbleUp maintains the heap property of parents of ind.
    void bubbleUp(std::size_t ind)
    {
        std::size_t parent = ind/2;
        if (parent < 1) {
            return; // Already at root.
        }
        if (comp_fn(tree[ind], tree[parent])) {
            // Bubbble up if child less than parent.
            std::swap(tree[ind], tree[parent]);
            return bubbleUp(parent);
        }
    }

    // bubbleDown maintains the heap property of children of ind.
    void bubbleDown(std::size_t ind)
    {
        std::size_t lchild = ind*2;
        if (lchild < tree.size()) {
            std::size_t min = lchild, rchild = lchild+1;
            if (rchild < tree.size() && comp_fn(tree[rchild], tree[lchild])) {
                min = rchild;
            }
            if (comp_fn(tree[min], tree[ind])) {
                // Bubble down if child less than parent.
                std::swap(tree[ind], tree[min]);
                return bubbleDown(min);
            }
        }
    }
};

}

TEST_CASE("[MinHeap]")
{
    using namespace containers;

    using T = int;
    using Compare = std::less<int>;
    Heap<T, Compare> heap;
    REQUIRE(heap.size() == 0);

    // Push elements in ascending order: 10, 20, 30.
    heap.push(10);
    REQUIRE(heap.size() == 1);
    REQUIRE(heap.top() == 10);

    heap.push(20);
    REQUIRE(heap.size() == 2);
    REQUIRE(heap.top() == 10);

    heap.push(30);
    REQUIRE(heap.size() == 3);
    REQUIRE(heap.top() == 10);

    // Successively pop until heap is empty.
    heap.pop();
    REQUIRE(heap.size() == 2);
    REQUIRE(heap.top() == 20);

    heap.pop();
    REQUIRE(heap.size() == 1);
    REQUIRE(heap.top() == 30);

    heap.pop();
    REQUIRE(heap.size() == 0);
    REQUIRE_THROWS_AS(heap.top(), heap_empty_error);

    // Push elements in descending order: 30, 20, 10.
    heap.push(30);
    REQUIRE(heap.size() == 1);
    REQUIRE(heap.top() == 30);

    heap.push(20);
    REQUIRE(heap.size() == 2);
    REQUIRE(heap.top() == 20);

    heap.push(10);
    REQUIRE(heap.size() == 3);
    REQUIRE(heap.top() == 10);

    // Successively pop until heap is empty.
    heap.pop();
    REQUIRE(heap.size() == 2);
    REQUIRE(heap.top() == 20);

    heap.pop();
    REQUIRE(heap.size() == 1);
    REQUIRE(heap.top() == 30);

    heap.pop();
    REQUIRE(heap.size() == 0);
    REQUIRE_THROWS_AS(heap.top(), heap_empty_error);

    // Push unsorted elements: 20, 10, 30.
    heap.push(20);
    REQUIRE(heap.size() == 1);
    REQUIRE(heap.top() == 20);

    heap.push(10);
    REQUIRE(heap.size() == 2);
    REQUIRE(heap.top() == 10);

    heap.push(30);
    REQUIRE(heap.size() == 3);
    REQUIRE(heap.top() == 10);

    // Successively pop until heap is empty.
    heap.pop();
    REQUIRE(heap.size() == 2);
    REQUIRE(heap.top() == 20);

    heap.pop();
    REQUIRE(heap.size() == 1);
    REQUIRE(heap.top() == 30);

    heap.pop();
    REQUIRE(heap.size() == 0);
    REQUIRE_THROWS_AS(heap.top(), heap_empty_error);
}

TEST_CASE("[MaxHeap]")
{
    using namespace containers;

    using T = int;
    using Compare = std::greater<int>; // Invert sense of comparison.
    Heap<T, Compare> heap;
    REQUIRE(heap.size() == 0);

    // Push elements in ascending order: 10, 20, 30.
    heap.push(10);
    REQUIRE(heap.size() == 1);
    REQUIRE(heap.top() == 10);

    heap.push(20);
    REQUIRE(heap.size() == 2);
    REQUIRE(heap.top() == 20);

    heap.push(30);
    REQUIRE(heap.size() == 3);
    REQUIRE(heap.top() == 30);

    // Successively pop until heap is empty.
    heap.pop();
    REQUIRE(heap.size() == 2);
    REQUIRE(heap.top() == 20);

    heap.pop();
    REQUIRE(heap.size() == 1);
    REQUIRE(heap.top() == 10);

    heap.pop();
    REQUIRE(heap.size() == 0);
    REQUIRE_THROWS_AS(heap.top(), heap_empty_error);

    // Push elements in descending order: 30, 20, 10.
    heap.push(30);
    REQUIRE(heap.size() == 1);
    REQUIRE(heap.top() == 30);

    heap.push(20);
    REQUIRE(heap.size() == 2);
    REQUIRE(heap.top() == 30);

    heap.push(10);
    REQUIRE(heap.size() == 3);
    REQUIRE(heap.top() == 30);

    // Successively pop until heap is empty.
    heap.pop();
    REQUIRE(heap.size() == 2);
    REQUIRE(heap.top() == 20);

    heap.pop();
    REQUIRE(heap.size() == 1);
    REQUIRE(heap.top() == 10);

    heap.pop();
    REQUIRE(heap.size() == 0);
    REQUIRE_THROWS_AS(heap.top(), heap_empty_error);

    // Push unsorted elements: 20, 10, 30.
    heap.push(20);
    REQUIRE(heap.size() == 1);
    REQUIRE(heap.top() == 20);

    heap.push(10);
    REQUIRE(heap.size() == 2);
    REQUIRE(heap.top() == 20);

    heap.push(30);
    REQUIRE(heap.size() == 3);
    REQUIRE(heap.top() == 30);

    // Successively pop until heap is empty.
    heap.pop();
    REQUIRE(heap.size() == 2);
    REQUIRE(heap.top() == 20);

    heap.pop();
    REQUIRE(heap.size() == 1);
    REQUIRE(heap.top() == 10);

    heap.pop();
    REQUIRE(heap.size() == 0);
    REQUIRE_THROWS_AS(heap.top(), heap_empty_error);
}
