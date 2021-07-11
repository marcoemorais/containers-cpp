#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#if __has_include("containers/print.h")
#   include "containers/print.h"
#endif

namespace containers
{

// UnionFind implements a disjoint-set for set membership lookup.
template <typename ObjectID = int>
class UnionFind
{
public:
    using ComponentID = ObjectID;

    UnionFind() = default;

    // add id1 and id2 and union the components of each set.
    void add(const ObjectID& id1, const ObjectID& id2)
    {
        // find the component of id1 and id2, add if not found.
        if (!components.count(id1)) {
            add(id1);
        }
        auto comp1 = find_component(id1);
        if (!components.count(id2)) {
            add(id2);
        }
        auto comp2 = find_component(id2);
        // union the components if not members of same set.
        if (comp1 != comp2) {
            // Merge the smaller set into the larger set.
            if (sizes[comp1] < sizes[comp2]) {
                components[comp1] = comp2;
                sizes[comp2] += sizes[comp1];
                path_compression(id1, comp2);
            }
            else {
                components[comp2] = comp1;
                sizes[comp1] += sizes[comp2];
                path_compression(id2, comp1);
            }
        }
    }

    // add id as a new component.
    void add(const ObjectID& id)
    {
        components[id] = id;
        sizes[id] = 1;
    }

    // is_member returns true when id1 and id2 are in same set.
    bool is_member(const ObjectID& id1, const ObjectID& id2) const
    {
        auto comp1 = find_component(id1);
        auto comp2 = find_component(id2);
        return comp1 == comp2;
    }

    // size returns the size of the component to which id belongs.
    std::size_t size(const ObjectID& id) const
    {
        auto comp = find_component(id);
        return sizes.at(comp);
    }

private:
    std::unordered_map<ObjectID, ComponentID> components;
    std::unordered_map<ComponentID, std::size_t> sizes;

    // find_component returns the component associated with id.
    ComponentID find_component(const ObjectID& id) const
    {
        auto comp = id;
        while (comp != components.at(comp)) {
            comp = components.at(comp);
        }
        return comp;
    }

    // path_compression moves objects along path from id to comp under comp.
    void path_compression(const ObjectID& id, const ComponentID& comp)
    {
        auto path = id;
        while (path != components[path]) {
            auto next = components[path]; // Save next object on path.
            components[path] = comp; // Move under comp.
            path = next;
        }
    }
};

}

TEST_CASE("[UnionFind]")
{
    using namespace containers;

    using ObjectID = int;

    UnionFind<ObjectID> uf;

    uf.add(1);
    REQUIRE(uf.size(1) == 1);

    uf.add(1, 2);
    REQUIRE(uf.is_member(1, 2) == true);
    REQUIRE(uf.size(1) == 2);
    REQUIRE(uf.size(2) == 2);

    uf.add(3, 4);
    REQUIRE(uf.is_member(3, 4) == true);
    REQUIRE(uf.is_member(1, 3) == false);
    REQUIRE(uf.is_member(1, 4) == false);
    REQUIRE(uf.is_member(2, 3) == false);
    REQUIRE(uf.is_member(2, 4) == false);
    REQUIRE(uf.size(3) == 2);
    REQUIRE(uf.size(4) == 2);

    uf.add(5, 2);
    REQUIRE(uf.is_member(1, 5) == true);
    REQUIRE(uf.is_member(2, 5) == true);
    REQUIRE(uf.is_member(3, 5) == false);
    REQUIRE(uf.is_member(4, 5) == false);
    REQUIRE(uf.size(1) == 3);
    REQUIRE(uf.size(2) == 3);
    REQUIRE(uf.size(5) == 3);

    uf.add(6);
    REQUIRE(uf.is_member(1, 6) == false);
    REQUIRE(uf.is_member(2, 6) == false);
    REQUIRE(uf.is_member(3, 6) == false);
    REQUIRE(uf.is_member(4, 6) == false);
    REQUIRE(uf.is_member(5, 6) == false);
    REQUIRE(uf.size(6) == 1);

    uf.add(7, 6);
    REQUIRE(uf.is_member(7, 6) == true);
    REQUIRE(uf.size(6) == 2);
    REQUIRE(uf.size(7) == 2);

    uf.add(1, 6);
    REQUIRE(uf.is_member(1, 2) == true);
    REQUIRE(uf.is_member(2, 3) == false);
    REQUIRE(uf.is_member(3, 4) == true);
    REQUIRE(uf.is_member(4, 5) == false);
    REQUIRE(uf.is_member(2, 5) == true);
    REQUIRE(uf.is_member(2, 6) == true);
    REQUIRE(uf.is_member(2, 7) == true);
    REQUIRE(uf.size(1) == 5);
    REQUIRE(uf.size(2) == 5);
    REQUIRE(uf.size(3) == 2);
    REQUIRE(uf.size(4) == 2);
    REQUIRE(uf.size(5) == 5);
    REQUIRE(uf.size(6) == 5);
    REQUIRE(uf.size(7) == 5);

    uf.add(3, 7);
    REQUIRE(uf.is_member(1, 2) == true);
    REQUIRE(uf.is_member(2, 3) == true);
    REQUIRE(uf.is_member(3, 4) == true);
    REQUIRE(uf.is_member(4, 5) == true);
    REQUIRE(uf.is_member(5, 6) == true);
    REQUIRE(uf.is_member(6, 7) == true);
    REQUIRE(uf.size(1) == 7);
    REQUIRE(uf.size(2) == 7);
    REQUIRE(uf.size(3) == 7);
    REQUIRE(uf.size(4) == 7);
    REQUIRE(uf.size(5) == 7);
    REQUIRE(uf.size(6) == 7);
    REQUIRE(uf.size(7) == 7);
}
