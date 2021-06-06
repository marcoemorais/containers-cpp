#include <functional>
#include <memory>
#include <vector>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#if __has_include("containers/print.h")
#   include "containers/print.h"
#endif

namespace containers
{

// BSTNode is a node in a binary search tree.
template <typename T>
struct BSTNode
{
    using BSTNodePtr = std::shared_ptr<BSTNode<T>>;

    BSTNode() = default;

    BSTNode(const T& data) : data(data) { }

    BSTNode<T>* insert(const T& v)
    {
        if (v < data) {
            if (left) {
                return left->insert(v);
            }
            left = std::make_shared<BSTNode<T>>(v);
            return left.get();
        }
        else if (v > data) {
            if (right) {
                return right->insert(v);
            }
            right = std::make_shared<BSTNode<T>>(v);
            return right.get();
        }
        return this; // Ignore dupes.
    }

    T data;
    BSTNodePtr left;
    BSTNodePtr right;
};

// make_bst returns bst initialized from container of values.
template <typename T>
std::shared_ptr<BSTNode<T>>
make_bst(const std::vector<T>& values)
{
    std::shared_ptr<BSTNode<T>> root;

    for (const auto& v : values) {
        if (!root) {
            root = std::make_shared<BSTNode<T>>(v);
        }
        else {
            root->insert(v);
        }
    }

    return root;
}

// Order is a traversal order.
enum Order { preorder, inorder, postorder };

// make_vector returns vector initialized from tree.
template <typename T>
std::vector<T>
make_vector(const BSTNode<T>* root, const Order& order=inorder)
{
    std::vector<T> values;
    if (!root) {
        return values;
    }

    using VisitFunc = std::function<void(const BSTNode<T>*)>;

    // Insert elements according to requested traversal order.
    switch(order) {
    case Order::preorder:
        {
        VisitFunc visit_pre = [&values, &visit_pre](const BSTNode<T>* node) {
            values.push_back(node->data);
            if (node->left) {
                visit_pre(node->left.get());
            }
            if (node->right) {
                visit_pre(node->right.get());
            }
        };
        visit_pre(root);
        break;
        }
    case Order::inorder:
        {
        VisitFunc visit_in = [&values, &visit_in](const BSTNode<T>* node) {
            if (node->left) {
                visit_in(node->left.get());
            }
            values.push_back(node->data);
            if (node->right) {
                visit_in(node->right.get());
            }
        };
        visit_in(root);
        break;
        }
    case Order::postorder:
        {
        VisitFunc visit_post = [&values, &visit_post](const BSTNode<T>* node) {
            if (node->left) {
                visit_post(node->left.get());
            }
            if (node->right) {
                visit_post(node->right.get());
            }
            values.push_back(node->data);
        };
        visit_post(root);
        break;
        }
    }

    return values;
}

// find returns the node containig value or nullptr if not in tree.
template <typename T>
const BSTNode<T>*
find(const BSTNode<T>* root, const T& v)
{
    if (!root) {
        return nullptr;
    }
    if (v < root->data) {
        if (root->left) {
            return find(root->left.get(), v);
        }
        return nullptr;
    }
    else if (v > root->data) {
        if (root->right) {
            return find(root->right.get(), v);
        }
        return nullptr;
    }
    return root; // v == data.
}

}

TEST_CASE("[make_bst]")
{
    using namespace containers;

    struct test_case
    {
        std::string name;
        std::vector<int> values;
        std::vector<int> preorder;
        std::vector<int> inorder;
        std::vector<int> postorder;
    };

    std::vector<test_case> test_cases{
        {
            "Empty tree.",
            {},
            {}, // pre
            {}, // in
            {}, // post
        },
        {
            "1 node.",
            {1},
            {1}, // pre
            {1}, // in
            {1}, // post
        },
        {
            "3 node, ascending.",
            {1, 2, 3},
            {1, 2, 3}, // pre
            {1, 2, 3}, // in
            {3, 2, 1}, // post
        },
        {
            "3 node, descending.",
            {3, 2, 1},
            {3, 2, 1}, // pre
            {1, 2, 3}, // in
            {1, 2, 3}, // post
        },
        {
            "3 node, random.",
            {2, 1, 3},
            {2, 1, 3}, // pre
            {1, 2, 3}, // in
            {1, 3, 2}, // post
        },
        {
            "7 node, balanced.",
            {4, 2, 1, 3, 6, 5, 7},
            {4, 2, 1, 3, 6, 5, 7}, // pre
            {1, 2, 3, 4, 5, 6, 7}, // in
            {1, 3, 2, 5, 7, 6, 4}, // post
        },
    };

    for (const auto& c : test_cases) {
        INFO(c.name);
        auto root = make_bst(c.values);
        {
            auto rcv = make_vector(root.get(), Order::preorder);
            REQUIRE(rcv == c.preorder);
        }
        {
            auto rcv = make_vector(root.get(), Order::inorder);
            REQUIRE(rcv == c.inorder);
        }
        {
            auto rcv = make_vector(root.get(), Order::postorder);
            REQUIRE(rcv == c.postorder);
        }
    }
}

TEST_CASE("[find]")
{
    using namespace containers;

    // FindResult holds searched for value and expected result.
    using FindResult = std::pair<int, bool>;

    struct test_case
    {
        std::string name;
        std::vector<int> values;
        std::vector<FindResult> expected;
    };

    std::vector<test_case> test_cases{
        {
            "Balanced tree.",
            {7, 3, 1, 5, 11, 9, 13},
            {
                {0, false},
                {1, true},
                {2, false},
                {3, true},
                {4, false},
                {5, true},
                {6, false},
                {7, true},
                {8, false},
                {9, true},
                {10, false},
                {11, true},
                {12, false},
                {13, true},
                {14, false},
            },
        },
    };

    for (const auto& c : test_cases) {
        INFO(c.name);
        auto root = make_bst(c.values);
        for (const auto& expect : c.expected) {
            auto rcv = find(root.get(), expect.first);
            if (expect.second) {
                REQUIRE(rcv != nullptr);
                REQUIRE(rcv->data == expect.first);
            }
            else {
                REQUIRE(rcv == nullptr);
            }
        }
    }
}
