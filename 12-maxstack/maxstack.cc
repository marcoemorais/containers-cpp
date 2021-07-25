#include <cstddef>
#include <stack>
#include <stdexcept>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#if __has_include("containers/print.h")
#   include "containers/print.h"
#endif

namespace containers
{

struct stack_empty_error : std::runtime_error
{
    using std::runtime_error::runtime_error;
};

// MaxStack is a LIFO container with constant time access to maximum value.
template <typename T>
class MaxStack
{
public:
    MaxStack() = default;

    // push adds element to the top of stack
    void push(const T& v)
    {
        elems.push(v);
        if (maxv.empty() || v >= maxv.top()) {
            maxv.push(v);
        }
    }

    // top returns but does not remove top of stack.
    const T& top() const
    {
        if (elems.empty()) {
            throw stack_empty_error{"top from empty stack"};
        }
        return elems.top();
    }

    // max returns but does not remove maximum value in stack.
    const T& max() const
    {
        if (elems.empty()) {
            throw stack_empty_error{"max from empty stack"};
        }
        return maxv.top();
    }

    // pop removes but does not return top of stack.
    void pop()
    {
        if (elems.empty()) {
            throw stack_empty_error{"pop from empty stack"};
        }
        if (elems.top() == maxv.top()) {
            maxv.pop();
        }
        elems.pop();
    }

    // size returns number of elements in stack.
    std::size_t size() const
    {
        return elems.size();
    }

private:
    std::stack<T> elems;
    std::stack<T> maxv;
};

}

TEST_CASE("[MaxStack]")
{
    using namespace containers;

    using T = int;
    MaxStack<T> stack;
    REQUIRE(stack.size() == 0);
    REQUIRE_THROWS_AS(stack.top(), stack_empty_error);
    REQUIRE_THROWS_AS(stack.max(), stack_empty_error);
    REQUIRE_THROWS_AS(stack.pop(), stack_empty_error);

    // 1-3-2
    stack.push(1);
    REQUIRE(stack.top() == 1);
    REQUIRE(stack.max() == 1);
    REQUIRE(stack.size() == 1);

    stack.push(3);
    REQUIRE(stack.top() == 3);
    REQUIRE(stack.max() == 3);
    REQUIRE(stack.size() == 2);

    stack.push(2);
    REQUIRE(stack.top() == 2);
    REQUIRE(stack.max() == 3);
    REQUIRE(stack.size() == 3);

    stack.pop();
    REQUIRE(stack.top() == 3);
    REQUIRE(stack.max() == 3);
    REQUIRE(stack.size() == 2);

    stack.pop();
    REQUIRE(stack.top() == 1);
    REQUIRE(stack.max() == 1);
    REQUIRE(stack.size() == 1);

    stack.pop();
    REQUIRE(stack.size() == 0);
    REQUIRE_THROWS_AS(stack.top(), stack_empty_error);
    REQUIRE_THROWS_AS(stack.max(), stack_empty_error);

    // 1-2-3
    stack.push(1);
    REQUIRE(stack.top() == 1);
    REQUIRE(stack.max() == 1);
    REQUIRE(stack.size() == 1);

    stack.push(2);
    REQUIRE(stack.top() == 2);
    REQUIRE(stack.max() == 2);
    REQUIRE(stack.size() == 2);

    stack.push(3);
    REQUIRE(stack.top() == 3);
    REQUIRE(stack.max() == 3);
    REQUIRE(stack.size() == 3);

    stack.pop();
    REQUIRE(stack.top() == 2);
    REQUIRE(stack.max() == 2);
    REQUIRE(stack.size() == 2);

    stack.pop();
    REQUIRE(stack.top() == 1);
    REQUIRE(stack.max() == 1);
    REQUIRE(stack.size() == 1);

    stack.pop();
    REQUIRE(stack.size() == 0);
    REQUIRE_THROWS_AS(stack.top(), stack_empty_error);
    REQUIRE_THROWS_AS(stack.max(), stack_empty_error);

    // 3-2-1
    stack.push(3);
    REQUIRE(stack.top() == 3);
    REQUIRE(stack.max() == 3);
    REQUIRE(stack.size() == 1);

    stack.push(2);
    REQUIRE(stack.top() == 2);
    REQUIRE(stack.max() == 3);
    REQUIRE(stack.size() == 2);

    stack.push(1);
    REQUIRE(stack.top() == 1);
    REQUIRE(stack.max() == 3);
    REQUIRE(stack.size() == 3);

    stack.pop();
    REQUIRE(stack.top() == 2);
    REQUIRE(stack.max() == 3);
    REQUIRE(stack.size() == 2);

    stack.pop();
    REQUIRE(stack.top() == 3);
    REQUIRE(stack.max() == 3);
    REQUIRE(stack.size() == 1);

    stack.pop();
    REQUIRE(stack.size() == 0);
    REQUIRE_THROWS_AS(stack.top(), stack_empty_error);
    REQUIRE_THROWS_AS(stack.max(), stack_empty_error);
}
