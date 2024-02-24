#include <cassert>
#include <cstddef>
#include <functional>
#include <iterator>
#include <list>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#if __has_include("containers/print.h")
#   include "containers/print.h"
#endif

namespace containers
{

// LRU supports constant time insert and retrieval.
template <typename Key,
          typename Value,
          typename Hash = std::hash<Key>,
          typename KeyEqual = std::equal_to<Key>>
class LRU
{
public:
    LRU() = default;

    LRU(size_t capacity) : capacity_(capacity) { assert(capacity > 0); }

    // get returns Value associated with Key or nullopt.
    std::optional<Value> get(const Key& key)
    {
        auto it = cache.find(key);
        if (it == std::end(cache)) {
            return std::nullopt;
        }
        // Update the access order for this key in list.
        order.erase(it->second.second);
        it->second.second = order.insert(std::end(order), key);
        return it->second.first;
    }

    // set adds Key and Value to cache.
    void set(const Key& key, const Value& value)
    {
        auto it = cache.find(key);
        if (it == std::end(cache)) {
            // Add new entry to cache.
            if (std::size(cache) == capacity_) {
                // Evict least recently used entry.
                cache.erase(order.front());
                order.pop_front();
            }
            cache.emplace(
                key,                                            // Key.
                make_pair(value,
                          order.insert(std::end(order), key))); // Value.
        }
        else {
            it->second.first = value; // Update Value.
            order.erase(it->second.second);
            it->second.second = order.insert(std::end(order), key);
        }
    }

    // size returns number of elements in cache.
    std::size_t size() const { return std::size(cache); }

    // capacity returns capacity of cache.
    std::size_t capacity() const { return capacity_; }

private:
    // CacheEntry is a pair of Value and order of Key in list.
    using CacheEntry = std::pair<Value, typename std::list<Key>::iterator>;

    // cache stores Key and CacheEntry.
    std::unordered_map<Key, CacheEntry> cache;

    // order stores Key access order.
    // Most recently accessed entries are at back of list.
    // Least recently accessed entries are at front of list.
    std::list<Key> order;

    // capacity is the upper bound on the number of elements in list.
    std::size_t capacity_{10};
};

}

TEST_CASE("[LRU]")
{
    using namespace containers;

    using Key = std::string;
    using Value = std::string;

    std::size_t capacity{3};
    LRU<Key, Value> lru(capacity);
    {
        REQUIRE(lru.capacity() == capacity);
        REQUIRE(lru.size() == 0);
    }

    // Fill to capacity.
    lru.set("k1", "v1");
    {
        REQUIRE(lru.size() == 1);

        auto v = lru.get("k1");
        REQUIRE(v.has_value() == true);
        REQUIRE(*v == "v1");
    }

    lru.set("k2", "v2");
    {
        REQUIRE(lru.size() == 2);

        auto v = lru.get("k2");
        REQUIRE(v.has_value() == true);
        REQUIRE(*v == "v2");
    }

    lru.set("k3", "v3");
    {
        REQUIRE(lru.size() == 3);

        auto v = lru.get("k3");
        REQUIRE(v.has_value() == true);
        REQUIRE(*v == "v3");
    }

    REQUIRE(lru.size() == lru.capacity());

    // Add k4 and evict k1.
    lru.set("k4", "v4");
    {
        {
            auto v = lru.get("k1");
            REQUIRE(v.has_value() == false);
        }
        {
            auto v = lru.get("k4");
            REQUIRE(v.has_value() == true);
            REQUIRE(*v == "v4");
        }
    }

    REQUIRE(lru.size() == lru.capacity());

    // Access k2 so that k3 becomes least recently used.
    {
        auto v = lru.get("k2");
        REQUIRE(v.has_value() == true);
        REQUIRE(*v == "v2");
    }

    // Add k5 and evict k3.
    lru.set("k5", "v5");
    {
        auto v = lru.get("k3");
        REQUIRE(v.has_value() == false);
    }

    // Update k4 so that k2 becomes least recently used.
    lru.set("k4", "v44");

    // Add k6 and evict k2.
    lru.set("k6", "v6");
    {
        auto v = lru.get("k2");
        REQUIRE(v.has_value() == false);
    }

    // Access remaining keys k4, k5, and k6.
    {
        {
            auto v = lru.get("k4");
            REQUIRE(v.has_value() == true);
            REQUIRE(*v == "v44");
        }
        {
            auto v = lru.get("k5");
            REQUIRE(v.has_value() == true);
            REQUIRE(*v == "v5");
        }
        {
            auto v = lru.get("k6");
            REQUIRE(v.has_value() == true);
            REQUIRE(*v == "v6");
        }
    }

    // Access non-existent key.
    {
        auto v = lru.get("does-not-exist");
        REQUIRE(v.has_value() == false);
    }

    REQUIRE(lru.size() == lru.capacity());
}
