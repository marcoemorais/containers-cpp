#include <algorithm>
#include <cstddef>
#include <memory>
#include <queue>
#include <vector>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#if __has_include("containers/print.h")
#   include "containers/print.h"
#endif

namespace containers
{

// TreeNode is a node in a binary tree.
template <typename T>
struct TreeNode
{
    using TreeNodePtr = std::shared_ptr<TreeNode<T>>;

    TreeNode() = default;

    TreeNode(const T& data)
        : data(data)
    { }

    TreeNode<T>* insert_left(const T& data)
    {
        left = std::make_shared<TreeNode<T>>(data);
        return left.get();
    }

    TreeNode<T>* insert_right(const T& data)
    {
        right = std::make_shared<TreeNode<T>>(data);
        return right.get();
    }

    T data;
    TreeNodePtr left;
    TreeNodePtr right;
};

// make_tree returns a tree initailized from container of values.
template <typename T>
std::shared_ptr<TreeNode<T>>
make_tree(const std::vector<T>& values)
{
    // Since this is a binary tree, there is no ordering wrt value.
    // Values are inserted in level-order as they appear in the
    // container of values; starting at root and growing downward
    // adding sibling nodes from left to right.
    std::shared_ptr<TreeNode<T>> root;

    std::queue<TreeNode<T>*> nodes;
    for (const auto& v : values) {
        if (nodes.empty()) {
            root = std::make_shared<TreeNode<T>>(v);
            nodes.push(root.get());
        }
        else {
            auto node = nodes.front();
            if (!node->left) {
                nodes.push(node->insert_left(v));
            }
            else {
                nodes.push(node->insert_right(v));
                nodes.pop(); // Both children populated.
            }
        }
    }

    return root;
}

// make_vector returns a vector initialized from tree.
template <typename T>
std::vector<T>
make_vector(const TreeNode<T>* root)
{
    // Since this is a binary tree, there is no ordering wrt value.
    // Values are inserted into the vector in the order they appear
    // during a level order traversal of the tree.
    std::vector<T> values;

    std::queue<const TreeNode<T>*> nodes;
    if (root) {
        nodes.push(root);
    }
    while (!nodes.empty()) {
        auto node = nodes.front();
        values.emplace_back(node->data);
        if (node->left) {
            nodes.push(node->left.get());
        }
        if (node->right) {
            nodes.push(node->right.get());
        }
        nodes.pop(); // Both children queued.
    }

    return values;
}

// tree_height returns the height of the tree.
template <typename T>
std::size_t
tree_height(const TreeNode<T>* root)
{
    if (!root || (!root->left && !root->right)) {
        // Null node or node with no children has 0 height.
        return 0;
    }
    return 1 + std::max(tree_height(root->left.get()), tree_height(root->right.get()));
}

// tree_size returns the number of elements in the tree.
template <typename T>
std::size_t
tree_size(const TreeNode<T>* root)
{
    if (!root) {
        return 0;
    }
    return 1 + tree_size(root->left.get()) + tree_size(root->right.get());
}

}

TEST_CASE("[make_tree]")
{
    using namespace containers;

    struct test_case
    {
        std::string name;
        std::vector<int> input; // Same as expected.
    };

    std::vector<test_case> test_cases{
        {
            "Empty tree.",
            {},
        },
        {
            "1 node.",
            {1},
        },
        {
            "2 node.",
            {1, 2},
        },
        {
            "3 node.",
            {1, 2, 3},
        },
        {
            "4 node.",
            {1, 2, 3, 4},
        },
        {
            "5 node.",
            {1, 2, 3, 4, 5},
        },
        {
            "6 node.",
            {1, 2, 3, 4, 5, 6},
        },
        {
            "7 node.",
            {1, 2, 3, 4, 5, 6, 7},
        }
    };

    for (const auto& c : test_cases) {
        auto root = make_tree(c.input);
        auto rootv = make_vector(root.get());
        REQUIRE(rootv == c.input); // input == expected
    }
}

TEST_CASE("[tree_height]")
{
    using namespace containers;

    struct test_case
    {
        std::string name;
        std::vector<int> input;
        std::size_t expected;
    };

    std::vector<test_case> test_cases{
        // Empty and 1 node trees have height=0.
        {
            "Empty tree.",
            {},
            0,
        },
        {
            "1 node.",
            {1},
            0,
        },
        {
            "2 node.",
            {1, 2},
            1,
        },
        {
            "3 node.",
            {1, 2, 3},
            1,
        },
        {
            "4 node.",
            {1, 2, 3, 4},
            2,
        },
        {
            "5 node.",
            {1, 2, 3, 4, 5},
            2,
        },
        {
            "6 node.",
            {1, 2, 3, 4, 5, 6},
            2,
        },
        {
            "7 node.",
            {1, 2, 3, 4, 5, 6, 7},
            2,
        },
        {
            "8 node.",
            {1, 2, 3, 4, 5, 6, 7, 8},
            3,
        }
    };

    for (const auto& c : test_cases) {
        auto root = make_tree(c.input);
        auto height = tree_height(root.get());
        REQUIRE(height == c.expected);
    }
}

TEST_CASE("[tree_size]")
{
    using namespace containers;

    struct test_case
    {
        std::string name;
        std::vector<int> input;
        std::size_t expected;
    };

    std::vector<test_case> test_cases{
        // Empty and 1 node trees have height=0.
        {
            "Empty tree.",
            {},
            0,
        },
        {
            "1 node.",
            {1},
            1,
        },
        {
            "2 node.",
            {1, 2},
            2,
        },
        {
            "3 node.",
            {1, 2, 3},
            3,
        },
        {
            "4 node.",
            {1, 2, 3, 4},
            4,
        },
        {
            "5 node.",
            {1, 2, 3, 4, 5},
            5,
        },
        {
            "6 node.",
            {1, 2, 3, 4, 5, 6},
            6,
        },
        {
            "7 node.",
            {1, 2, 3, 4, 5, 6, 7},
            7,
        },
        {
            "8 node.",
            {1, 2, 3, 4, 5, 6, 7, 8},
            8,
        }
    };

    for (const auto& c : test_cases) {
        auto root = make_tree(c.input);
        auto size = tree_size(root.get());
        REQUIRE(size == c.expected);
    }
}
