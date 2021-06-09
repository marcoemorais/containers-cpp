#include <algorithm>
#include <forward_list>
#include <iterator>
#include <queue>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#if __has_include("containers/print.h")
#   include "containers/print.h"
#endif

namespace containers
{

// VertexID uniquely identifies a vertex in a graph.
using VertexID = int;

// Graph is an adjacency list representation of an unweighted graph.
struct Graph
{
    using VertexList = std::forward_list<VertexID>;

    Graph() = default;

    void add_edge(const VertexID& from, const VertexID& to, bool directed=false)
    {
        auto vlist = vertices.find(from);
        if (vlist != end(vertices)) {
            auto vto = std::find(std::begin(vlist->second),
                                 std::end(vlist->second),
                                 to);
            if (vto == std::end(vlist->second)) {
                // Insert `to` if not already in the list.
                vlist->second.push_front(to);
            }
        }
        else {
            vertices[from] = VertexList{to}; // New list.
        }
        if (!directed) {
            add_edge(to, from, true);
        }
        else {
            if (vertices.count(to) < 1) {
                // Add an empty list for `to` if not already in graph.
                vertices[to] = VertexList{};
            }
        }
    }

    std::unordered_map<VertexID, VertexList> vertices;
};

// bfs performs a breadth first search of the graph calling visit.
template <typename VisitFunc>
void
bfs(const Graph& graph, const VertexID& start, VisitFunc visit)
{
    std::unordered_set<VertexID> discovered;
    using QueueEntry = std::pair<VertexID, VertexID>; // (from,to)
    std::queue<QueueEntry> vertices;
    vertices.emplace(start, start);
    discovered.insert(start);

    while (!vertices.empty()) {
        auto vpair = vertices.front();
        vertices.pop(); // Discard.
        visit(vpair.first, vpair.second); // Visit vertex.
        for (const auto& vto : graph.vertices.at(vpair.second)) {
            if (discovered.count(vto) < 1) {
                // Discovered new vertex, add to queue.
                discovered.insert(vto);
                vertices.emplace(vpair.second, vto); // (from,to)
            }
        }
    }
}

}

TEST_CASE("[bfs]")
{
    using namespace containers;

    using AddEdgeParams = std::tuple<VertexID, VertexID>;

    struct test_case
    {
        std::string name;
        std::vector<AddEdgeParams> edges;
        bool directed;
        VertexID start;
        std::unordered_map<VertexID, int> expected;
    };

    std::vector<test_case> test_cases{
        {
            "Undirected graph.",
            {
                {0, 1},
                {0, 2},
                {1, 3},
                {2, 3},
                {2, 4},
                {3, 4},
                {3, 5},
                {4, 5},
            },
            false,
            0,
            {
                {0, 0},
                {1, 1},
                {2, 1},
                {3, 2},
                {4, 2},
                {5, 3},
            },
        },
        {
            "Directed acyclic graph.",
            {
                {0, 1},
                {0, 2},
                {1, 3},
                {2, 3},
                {3, 4},
                {3, 5},
                {3, 6}
            },
            true,
            0,
            {
                {0, 0},
                {1, 1},
                {2, 1},
                {3, 2},
                {4, 3},
                {5, 3},
                {6, 3},
            },
        },
        {
            "Directed graph with cycle.",
            {
                {1, 2},
                {1, 5},
                {2, 3},
                {2, 4},
                {2, 5},
                {4, 5},
                {4, 6},
                {6, 1},
            },
            true,
            1,
            {
                {1, 0},
                {2, 1},
                {3, 2},
                {4, 2},
                {5, 1},
                {6, 3},
            },
        },
    };

    for (const auto& c : test_cases) {
        INFO(c.name);
        Graph g;
        for (const auto& e : c.edges) {
            g.add_edge(std::get<0>(e), std::get<1>(e), c.directed);
        }
        // Visit function populates distance from start for each visited.
        std::unordered_map<VertexID, int> visited;
        auto visit_func =
            [&visited] (const VertexID& from, const VertexID& to) {
                if (from != to) {
                    // Increment distance along path by 1.
                    visited[to] = visited[from] + 1;
                }
            };
        bfs(g, c.start, visit_func);
        REQUIRE(visited == c.expected);
    }
}
