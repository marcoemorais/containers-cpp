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
          typename Less = std::less<T>>
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

    // front returns but does not remove from front of heap.
    T& front()
    {
        if (size() < 1) {
            throw heap_empty_error{"front from empty heap"};
        }
        return tree[1];
    }

    // pop_front removes but does not return from front of heap.
    void pop_front()
    {
        if (size() < 1) {
            throw heap_empty_error{"pop_front from empty heap"};
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
    std::vector<T> tree{T{0}};

    Less less; // Comparison function.

    // bubbleUp maintains the heap property of parents of ind.
    void bubbleUp(std::size_t ind)
    {
        std::size_t parent = ind/2;
        if (parent < 1) {
            return; // Already at root.
        }
        if (less(tree[ind], tree[parent])) {
            // Bubbble up if child less than parent.
            std::swap(tree[ind], tree[parent]);
            return bubbleUp(parent);
        }
    }

    // bubbleDown maintains the heap property of children of ind.
    void bubbleDown(std::size_t ind)
    {
        std::size_t lchild = ind*2;
        if (lchild >= tree.size()) {
            return; // Already at leaf.
        }
        std::size_t min = lchild;
        if (lchild+1 < tree.size() && less(tree[lchild+1], tree[lchild])) {
            min = lchild+1;
        }
        if (less(tree[min], tree[ind])) {
            // Bubble down if child less than parent.
            std::swap(tree[ind], tree[min]);
            return bubbleDown(min);
        }
    }
};

}

TEST_CASE("[MinHeap]")
{
    using namespace containers;

    using T = int;
    using Less = std::less<int>;
    Heap<T, Less> heap;
    REQUIRE(heap.size() == 0);

    // Push elements in ascending order: 10, 20, 30.
    heap.push(10);
    REQUIRE(heap.size() == 1);
    REQUIRE(heap.front() == 10);

    heap.push(20);
    REQUIRE(heap.size() == 2);
    REQUIRE(heap.front() == 10);

    heap.push(30);
    REQUIRE(heap.size() == 3);
    REQUIRE(heap.front() == 10);

    // Successively pop until heap is empty.
    heap.pop_front();
    REQUIRE(heap.size() == 2);
    REQUIRE(heap.front() == 20);

    heap.pop_front();
    REQUIRE(heap.size() == 1);
    REQUIRE(heap.front() == 30);

    heap.pop_front();
    REQUIRE(heap.size() == 0);
    REQUIRE_THROWS_AS(heap.front(), heap_empty_error);

    // Push elements in descending order: 30, 20, 10.
    heap.push(30);
    REQUIRE(heap.size() == 1);
    REQUIRE(heap.front() == 30);

    heap.push(20);
    REQUIRE(heap.size() == 2);
    REQUIRE(heap.front() == 20);

    heap.push(10);
    REQUIRE(heap.size() == 3);
    REQUIRE(heap.front() == 10);

    // Successively pop until heap is empty.
    heap.pop_front();
    REQUIRE(heap.size() == 2);
    REQUIRE(heap.front() == 20);

    heap.pop_front();
    REQUIRE(heap.size() == 1);
    REQUIRE(heap.front() == 30);

    heap.pop_front();
    REQUIRE(heap.size() == 0);
    REQUIRE_THROWS_AS(heap.front(), heap_empty_error);

    // Push unsorted elements: 20, 10, 30.
    heap.push(20);
    REQUIRE(heap.size() == 1);
    REQUIRE(heap.front() == 20);

    heap.push(10);
    REQUIRE(heap.size() == 2);
    REQUIRE(heap.front() == 10);

    heap.push(30);
    REQUIRE(heap.size() == 3);
    REQUIRE(heap.front() == 10);

    // Successively pop until heap is empty.
    heap.pop_front();
    REQUIRE(heap.size() == 2);
    REQUIRE(heap.front() == 20);

    heap.pop_front();
    REQUIRE(heap.size() == 1);
    REQUIRE(heap.front() == 30);

    heap.pop_front();
    REQUIRE(heap.size() == 0);
    REQUIRE_THROWS_AS(heap.front(), heap_empty_error);
}

TEST_CASE("[MaxHeap]")
{
    using namespace containers;

    using T = int;
    using Less = std::greater<int>; // Invert sense of comparison.
    Heap<T, Less> heap;
    REQUIRE(heap.size() == 0);

    // Push elements in ascending order: 10, 20, 30.
    heap.push(10);
    REQUIRE(heap.size() == 1);
    REQUIRE(heap.front() == 10);

    heap.push(20);
    REQUIRE(heap.size() == 2);
    REQUIRE(heap.front() == 20);

    heap.push(30);
    REQUIRE(heap.size() == 3);
    REQUIRE(heap.front() == 30);

    // Successively pop until heap is empty.
    heap.pop_front();
    REQUIRE(heap.size() == 2);
    REQUIRE(heap.front() == 20);

    heap.pop_front();
    REQUIRE(heap.size() == 1);
    REQUIRE(heap.front() == 10);

    heap.pop_front();
    REQUIRE(heap.size() == 0);
    REQUIRE_THROWS_AS(heap.front(), heap_empty_error);

    // Push elements in descending order: 30, 20, 10.
    heap.push(30);
    REQUIRE(heap.size() == 1);
    REQUIRE(heap.front() == 30);

    heap.push(20);
    REQUIRE(heap.size() == 2);
    REQUIRE(heap.front() == 30);

    heap.push(10);
    REQUIRE(heap.size() == 3);
    REQUIRE(heap.front() == 30);

    // Successively pop until heap is empty.
    heap.pop_front();
    REQUIRE(heap.size() == 2);
    REQUIRE(heap.front() == 20);

    heap.pop_front();
    REQUIRE(heap.size() == 1);
    REQUIRE(heap.front() == 10);

    heap.pop_front();
    REQUIRE(heap.size() == 0);
    REQUIRE_THROWS_AS(heap.front(), heap_empty_error);

    // Push unsorted elements: 20, 10, 30.
    heap.push(20);
    REQUIRE(heap.size() == 1);
    REQUIRE(heap.front() == 20);

    heap.push(10);
    REQUIRE(heap.size() == 2);
    REQUIRE(heap.front() == 20);

    heap.push(30);
    REQUIRE(heap.size() == 3);
    REQUIRE(heap.front() == 30);

    // Successively pop until heap is empty.
    heap.pop_front();
    REQUIRE(heap.size() == 2);
    REQUIRE(heap.front() == 20);

    heap.pop_front();
    REQUIRE(heap.size() == 1);
    REQUIRE(heap.front() == 10);

    heap.pop_front();
    REQUIRE(heap.size() == 0);
    REQUIRE_THROWS_AS(heap.front(), heap_empty_error);
}
