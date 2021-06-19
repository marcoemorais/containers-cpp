#include <algorithm>
#include <cstddef>
#include <forward_list>
#include <functional>
#include <iterator>
#include <optional>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#if __has_include("containers/print.h")
#   include "containers/print.h"
#endif

namespace containers
{

// Hashtable supports constant time insert, retrieval and delete.
template <typename Key,
          typename Value,
          typename Hash = std::hash<Key>,
          typename KeyEqual = std::equal_to<Key>>
class Hashtable
{
public:
    using value_type = std::pair<Key, Value>;
    using BucketList = std::forward_list<value_type>;
    using BucketListIter = typename BucketList::iterator;
    using BucketListCIter = typename BucketList::const_iterator;

    Hashtable() : buckets(8) {}

    // insert adds entry to Hashtable.
    void insert(const Key& k, const Value& v)
    {
        auto h = hasher(k) % buckets.size();
        auto& b = buckets[h];
        auto entry = find(b, k);
        if (entry == std::end(b)) {
            // Add entry to the front of the chain.
            b.push_front(std::make_pair(k, v));
            ++nelems;
            // Check whether number of elements triggers rehash.
            if (nelems/float(buckets.size()) >= alpha) {
                rehash(buckets.size()*2);
            }
        }
        else {
            // Entry already exists in bucket, update value.
            entry->second = v;
        }
    }

    // find returns Value asssociated with k or nullopt.
    std::optional<Value> find(const Key& k) const
    {
        auto h = hasher(k) % buckets.size();
        auto b = buckets[h];
        auto entry = find(b, k);
        if (entry == std::end(b)) {
            return std::nullopt;
        }
        return entry->second;
    }

    // erase removes entry with Key from Hashtable.
    void erase(const Key& k)
    {
        auto h = hasher(k) % buckets.size();
        auto& b = buckets[h];
        int count = 0; // Count of elements removed.
        b.remove_if(
            [&k, &count, this](const value_type& v) {
                bool rm = equals(k, v.first);
                if (rm) {
                    ++count;
                }
                return rm;
            }
        );
        if (count) {
            --nelems;
        }
    }

    // size returns number of elements in Hashtable.
    std::size_t size() const
    {
        return nelems;
    }

    // nbuckets returns number of buckets in Hashtable.
    std::size_t nbuckets() const
    {
        return buckets.size();
    }

private:
    // buckets implements a Hashtable as vector of linked lists.
    std::vector<BucketList> buckets;

    // size is the count of entries in the hashtable.
    std::size_t nelems{0};

    // alpha is the load factor for triggering a rehash.
    float alpha{0.75};

    // hasher is the hash function used to hash key to bucket.
    Hash hasher;

    // equals is the equality function for Keys in the same bucket.
    KeyEqual equals;

    // find returns entry matching Key in bucket.
    BucketListIter find(BucketList& b, const Key& k) const
    {
        return std::find_if(
            std::begin(b), std::end(b),
            [&k, this](const value_type& v) { return equals(k, v.first); });
    }

    // rehash sets the number of buckets and rehashes all elements.
    void rehash(const std::size_t count)
    {
        // Copy every entry from buckets to buckets_cp.
        std::vector<BucketList> buckets_cp(count);
        buckets_cp.shrink_to_fit();
        for (const auto& b : buckets) {
            for (const auto& e : b) {
                auto h = hasher(e.first) % count;
                buckets_cp[h].emplace_front(e);
            }
        }
        buckets.swap(buckets_cp);
    }
};

}

TEST_CASE("[Hashtable]")
{
    using namespace containers;

    using Key = std::string;
    using Value = int;
    Hashtable<Key, Value> hashtable;

    REQUIRE(hashtable.size() == 0);

    // Insert entries.
    hashtable.insert("k1", 1);
    REQUIRE(hashtable.size() == 1);
    {
        auto rcv = hashtable.find("k1");
        REQUIRE(rcv.has_value() == true);
        REQUIRE(*rcv == 1);
    }

    hashtable.insert("k2", 2);
    REQUIRE(hashtable.size() == 2);
    {
        auto rcv = hashtable.find("k2");
        REQUIRE(rcv.has_value() == true);
        REQUIRE(*rcv == 2);
    }

    hashtable.insert("k3", 3);
    REQUIRE(hashtable.size() == 3);
    {
        auto rcv = hashtable.find("k3");
        REQUIRE(rcv.has_value() == true);
        REQUIRE(*rcv == 3);
    }

    // Update entry.
    hashtable.insert("k3", 4);
    REQUIRE(hashtable.size() == 3);
    {
        auto rcv = hashtable.find("k3");
        REQUIRE(rcv.has_value() == true);
        REQUIRE(*rcv == 4);
    }

    // Negative test.
    {
        auto rcv = hashtable.find("notfound");
        REQUIRE(rcv.has_value() == false);
    }

    // Erase entries.
    hashtable.erase("k3");
    REQUIRE(hashtable.size() == 2);
    {
        auto rcv = hashtable.find("k3");
        REQUIRE(rcv.has_value() == false);
    }

    hashtable.erase("k2");
    REQUIRE(hashtable.size() == 1);
    {
        auto rcv = hashtable.find("k2");
        REQUIRE(rcv.has_value() == false);
    }

    hashtable.erase("k1");
    REQUIRE(hashtable.size() == 0);
    {
        auto rcv = hashtable.find("k1");
        REQUIRE(rcv.has_value() == false);
    }

    // Force a rehash.
    std::vector<std::tuple<Key, Value, std::size_t>> entries{
        {"k1", 1, 8},
        {"k2", 2, 8},
        {"k3", 3, 8},
        {"k4", 4, 8},
        {"k5", 5, 8},
        {"k6", 6, 16},
        {"k7", 7, 16},
        {"k8", 8, 16},
        {"k9", 9, 16},
    };
    for (const auto& e : entries) {
        INFO(std::get<0>(e));
        hashtable.insert(std::get<0>(e), std::get<1>(e));
        auto rcv = hashtable.find(std::get<0>(e));
        REQUIRE(rcv.has_value() == true);
        REQUIRE(*rcv == std::get<1>(e));
        REQUIRE(hashtable.nbuckets() == std::get<2>(e));
    }
}
