#include <iterator>
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#if __has_include("containers/print.h")
#   include "containers/print.h"
#endif

namespace containers
{

// TrieNode is a node in a trie.
struct TrieNode
{
    using CharT = std::string::value_type;
    using TrieNodePtr = std::shared_ptr<TrieNode>;

    // Terminal character.
    static constexpr CharT TERMINAL = '\0';

    TrieNode() = default;

    TrieNode(CharT c) : c(c) { }

    // insert adds word to trie.
    void insert(const std::string& word)
    {
        if (word.empty()) {
            return;
        }

        // Search for first character of word in list of children.
        // If child is not found, create a new node in the trie.
        auto node = children.find(word[0]);
        if (node == end(children)) {
            auto result =
                children.emplace(word[0],
                                 std::make_shared<TrieNode>(word[0]));
            node = result.first;
        }
        if (word.size() == 1) {
            // No more characters, add terminal to list of children.
            node->second->children[TERMINAL] = nullptr;
        }
        else {
            node->second->insert(word.substr(1));
        }
    }

    // find returns all words matching prefix.
    std::vector<std::string> find(const std::string& prefix) const
    {
        // Search for the end of the prefix in the tree.
        auto root = match(prefix);
        if (!root) {
            return {};
        }

        // Perform a level order traversal of the subtree.
        // Each entry of the queue used for traversal contains the
        // node and substring obtained from traversal so far.
        std::vector<std::string> matches;
        using QueueEntry = std::pair<const TrieNode*, std::string>;
        std::queue<QueueEntry> nodes;
        nodes.emplace(root, prefix);
        while (!nodes.empty()) {
            auto node = nodes.front();
            nodes.pop();
            for (const auto& child : node.first->children) {
                if (child.first == TERMINAL) {
                    matches.emplace_back(node.second);
                }
                else {
                    nodes.emplace(child.second.get(),
                                  node.second + child.first);
                }
            }
        }

        return matches;
    }

    // match returns the terminal node of the prefix or nullptr.
    const TrieNode* match(const std::string& prefix) const
    {
        if (prefix.empty()) {
            return nullptr;
        }

        // Recursively traverse the tree character-by-character
        // until no match found or end of prefix.
        auto node = children.find(prefix[0]);
        if (node == end(children)) {
            return nullptr; // No match found.
        }
        if (prefix.size() > 1) {
            return node->second->match(prefix.substr(1));
        }
        return node->second.get(); // Terminal node.
    }

    // Character this node represents.
    CharT c = TERMINAL;

    // Child nodes including terminal, if any.
    std::unordered_map<CharT, TrieNodePtr> children;
};

}

TEST_CASE("[trie]")
{
    using namespace containers;

    TrieNode trie;

    // Insert words into trie and after each insertion
    // verify a match for the word and all previously
    // inserted words.
    std::vector<std::string> words{
        "abc",
        "ab",
        "a",
        "te",
        "tea",
        "team",
        "xyz",
    };
    for (std::size_t i = 0; i < words.size(); ++i) {
        INFO(words[i]);
        trie.insert(words[i]);
        for (std::size_t j = 0; j <= i; ++j) {
            auto rcv = trie.match(words[j]);
            REQUIRE(rcv != nullptr);
        }
    }

    // Verify that the following negative matches
    // are not found in the trie.
    std::vector<std::string> negwords{
        "",     // Empty string never matches.
        "m",    // No match from 1st character.
        "ti",   // No match from 2nd character.
        "ten",  // No match from 3rd character.
        "teal", // No match from 4th character.
    };
    for (const auto& word : negwords) {
        INFO(word);
        auto rcv = trie.match(word);
        REQUIRE(rcv == nullptr);
    }
}

TEST_CASE("[find]")
{
    using namespace containers;

    // Fill the index.
    TrieNode trie;
    std::vector<std::string> words{
        "t",
        "the",
        "their",
        "there",
        "was",
        "waste",
    };
    for (const auto& word : words) {
        trie.insert(word);
    }

    struct test_case
    {
        std::string prefix;
        std::vector<std::string> expected;
    };

    std::vector<test_case> test_cases{
        {
            "t", {"t", "the", "there", "their"},
        },
        {
            "th", {"the", "there", "their"},
        },
        {
            "the", {"the", "there", "their"},
        },
        {
            "them", {},
        },
        {
            "thei", {"their"},
        },
        {
            "their", {"their"},
        },
        {
            "theirs", {}
        },
        {
            "ther", {"there"}
        },
        {
            "there", {"there"}
        },
        {
            "therein", {}
        },
        {
            "w", {"was", "waste"}
        },
        {
            "wb", {}
        },
        {
            "wa", {"was", "waste"}
        },
        {
            "was", {"was", "waste"}
        },
        {
            "wae", {}
        },
        {
            "wast", {"waste"}
        },
        {
            "waste", {"waste"}
        },
        {
            "wastes", {}
        },
        {
            "x", {}
        },
    };

    for (const auto& c : test_cases) {
        INFO(c.prefix);
        auto rcv = trie.find(c.prefix);
        REQUIRE(rcv == c.expected);
    }
}
