#include <memory>
#include <string>
#include <vector>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#if __has_include("containers/print.h")
#   include "containers/print.h"
#endif

namespace containers
{

// ListNode is a node in a single linked list.
template <typename T>
struct ListNode
{
    using ListNodePtr = std::shared_ptr<ListNode<T>>;

    ListNode() = default;

    ListNode(const T& data)
        : data(data)
    { }

    ListNode(const T& data, ListNodePtr next)
        : data(data)
        , next(next)
    { }

    ListNode<T>* insert_after(const T& data)
    {
        next = std::make_shared<ListNode<T>>(data, next);
        return next.get();
    }

    T data;
    ListNodePtr next;
};

// make_list returns a list initialized from container of values.
template <typename T>
std::shared_ptr<ListNode<T>>
make_list(const std::vector<T>& values)
{
    std::shared_ptr<ListNode<T>> head;
    ListNode<T>* node;

    for (const auto& v : values) {
        if (!head) {
            head = std::make_shared<ListNode<T>>(v);
            node = head.get();
        }
        else {
            node = node->insert_after(v);
        }
    }

    return head;
}

// make_vector returns a vector initialized from list.
template <typename T>
std::vector<T>
make_vector(const ListNode<T>* head)
{
    std::vector<T> values;
    const ListNode<T>* node = head;

    while (node) {
        values.emplace_back(node->data);
        node = node->next.get();
    }

    return values;
}

// reverse performs an in-place reversal of elements of the list.
template <typename T>
std::shared_ptr<ListNode<T>>
reverse_list(std::shared_ptr<ListNode<T>> head)
{
    std::shared_ptr<ListNode<T>> revhead(head);
    ListNode<T>* node = head.get();
    if (!node) {
        return revhead;
    }

    while (node->next) {
        auto tmp = node->next;
        node->next = node->next->next;
        tmp->next = revhead;
        revhead = tmp;
    }

    return revhead;
}

}

TEST_CASE("[make_list]")
{
    using namespace containers;

    struct test_case
    {
        std::string name;
        std::vector<int> input;
        std::vector<int> expected;
    };

    std::vector<test_case> test_cases{
        {
            "Empty list.",
            {},
            {},
        },
        {
            "1 element list.",
            {1},
            {1},
        },
        {
            "2 element list.",
            {1, 2},
            {1, 2},
        },
        {
            "3 element list.",
            {1, 2, 3},
            {1, 2, 3},
        },
        {
            "4 element list.",
            {1, 2, 3, 4},
            {1, 2, 3, 4},
        }
    };

    for (const auto& c : test_cases) {
        auto mylist = make_list(c.input);
        auto myvec = make_vector(mylist.get());
        REQUIRE(myvec == c.expected);
    }
}

TEST_CASE("[reverse_list]")
{
    using namespace containers;

    struct test_case
    {
        std::string name;
        std::vector<int> input;
        std::vector<int> expected;
    };

    std::vector<test_case> test_cases{
        {
            "Empty list.",
            {},
            {},
        },
        {
            "1 element list.",
            {1},
            {1},
        },
        {
            "2 element list.",
            {1, 2},
            {2, 1},
        },
        {
            "3 element list.",
            {1, 2, 3},
            {3, 2, 1},
        },
        {
            "4 element list.",
            {1, 2, 3, 4},
            {4, 3, 2, 1},
        }
    };

    for (const auto& c : test_cases) {
        auto fwd = make_list(c.input);
        auto rev = reverse_list(fwd);
        auto revv = make_vector(rev.get());
        REQUIRE(revv == c.expected);
    }
}
