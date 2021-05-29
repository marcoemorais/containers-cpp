#include <cstddef>
#include <stdexcept>
#include <vector>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

namespace containers
{

struct stack_empty_error : std::runtime_error
{
    using std::runtime_error::runtime_error;
};

// Stack is a LIFO container of elements.
template <typename T>
class Stack
{
public:
    Stack() = default;

    // push appends data to top of stack.
    void push(const T& data)
    {
        elems.emplace_back(data);
    }

    // pop removes but does not return from top of stack.
    void pop()
    {
        if (elems.empty()) {
            throw stack_empty_error{"pop from empty stack"};
        }
        elems.pop_back();
    }

    // return but do not remove top of stack.
    T& top()
    {
        if (elems.empty()) {
            throw stack_empty_error{"top from empty stack"};
        }
        return elems.back();
    }

    // size returns number of elements in stack.
    std::size_t size() const
    {
        return elems.size();
    }

private:
    std::vector<T> elems;
};

}

TEST_CASE("[Stack]")
{
    using namespace containers;

    using T = int;
    Stack<T> stack;
    REQUIRE(stack.size() == 0);
    REQUIRE_THROWS_AS(stack.pop(), stack_empty_error);

    stack.push(1);
    REQUIRE(stack.top() == 1);
    REQUIRE(stack.size() == 1);

    stack.push(2);
    REQUIRE(stack.top() == 2);
    REQUIRE(stack.size() == 2);

    stack.push(3);
    REQUIRE(stack.top() == 3);
    REQUIRE(stack.size() == 3);

    stack.pop();
    REQUIRE(stack.top() == 2);
    REQUIRE(stack.size() == 2);

    stack.pop();
    REQUIRE(stack.top() == 1);
    REQUIRE(stack.size() == 1);

    stack.pop();
    REQUIRE(stack.size() == 0);
    REQUIRE_THROWS_AS(stack.top(), stack_empty_error);
}
