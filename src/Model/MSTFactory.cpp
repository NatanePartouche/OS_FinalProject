#include "MSTFactory.hpp"
#include "Graph.hpp"
#include <algorithm>
#include <vector>
#include <tuple>
#include <queue>
#include <limits>

// Prim's Algorithm Solver
Graph PrimSolver::solveMST(Graph& graph) {
    int V = graph.getNumVertices();
    Graph mst(V);

    std::vector<bool> inMST(V, false);
    std::vector<int> key(V, std::numeric_limits<int>::max());
    std::vector<int> parent(V, -1);

    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> pq;

    key[0] = 0;
    pq.push({0, 0});

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();

        if (inMST[u]) continue;

        inMST[u] = true;

        for (const auto& [v, weight] : graph.getAdjList()[u]) {
            if (!inMST[v] && weight < key[v]) {
                key[v] = weight;
                pq.push({key[v], v});
                parent[v] = u;
            }
        }
    }

    int edgeCount = 0;
    for (int v = 1; v < V; ++v) {
        if (parent[v] != -1) {
            mst.add_edge(parent[v], v, key[v]);
            edgeCount++;
        }
    }

    if (edgeCount < V - 1) {
        return Graph(0); // No MST found
    }

    return mst;
}

// Kruskal's Algorithm Solver
Graph KruskalSolver::solveMST(Graph& graph) {
    Graph mst(graph.getNumVertices());
    std::vector<std::tuple<int, int, int>> edges;

    for (int u = 0; u < graph.getNumVertices(); ++u) {
        for (const auto& [v, weight] : graph.getAdjList()[u]) {
            if (u < v) {
                edges.emplace_back(weight, u, v);
            }
        }
    }

    std::sort(edges.begin(), edges.end());
    UnionFind uf(graph.getNumVertices());

    int edgeCount = 0;
    for (const auto& [weight, u, v] : edges) {
        if (uf.unionSets(u, v)) {
            mst.add_edge(u, v, weight);
            edgeCount++;
        }
    }

    if (edgeCount < graph.getNumVertices() - 1) {
        return Graph(0); // No MST found
    }

    return mst;
}

// Borůvka's Algorithm Solver
Graph BoruvkaSolver::solveMST(Graph& graph) {
    int V = graph.getNumVertices();
    Graph mst(V);

    std::vector<int> component(V);
    for (int i = 0; i < V; ++i) {
        component[i] = i;
    }

    int numComponents = V;
    UnionFind uf(V);

    int edgeCount = 0;

    // Loop until there is only one component or no further progress can be made
    while (numComponents > 1) {
        std::vector<std::pair<int, std::pair<int, int>>> cheapest(V, {-1, {-1, -1}});

        // Find the cheapest edges connecting each component
        for (int u = 0; u < V; ++u) {
            for (const auto& [v, weight] : graph.getAdjList()[u]) {
                int compU = uf.find(u);
                int compV = uf.find(v);

                if (compU != compV) {
                    if (cheapest[compU].first == -1 || cheapest[compU].first > weight) {
                        cheapest[compU] = {weight, {u, v}};
                    }
                    if (cheapest[compV].first == -1 || cheapest[compV].first > weight) {
                        cheapest[compV] = {weight, {v, u}};
                    }
                }
            }
        }

        bool merged = false;  // Track if any components are merged in this iteration

        // Add the cheapest edges to the MST
        for (int i = 0; i < V; ++i) {
            if (cheapest[i].first != -1) {
                int u = cheapest[i].second.first;
                int v = cheapest[i].second.second;
                int weight = cheapest[i].first;

                if (uf.unionSets(u, v)) {
                    mst.add_edge(u, v, weight);
                    --numComponents;
                    edgeCount++;
                    merged = true;  // A merge happened, so progress was made
                }
            }
        }

        // If no components were merged and we still have more than one component, stop
        if (!merged && numComponents > 1) {
            return Graph(0);  // No MST found
        }
    }

    // If the number of edges is less than V-1, return an empty graph (no MST)
    if (edgeCount < V - 1) {
        return Graph(0);  // No MST found
    }

    return mst;
}

// Tarjan's Algorithm Solver
Graph TarjanSolver::solveMST(Graph& graph) {
    Graph mst(graph.getNumVertices());
    std::vector<std::tuple<int, int, int>> edges;

    for (int u = 0; u < graph.getNumVertices(); ++u) {
        for (const auto& [v, weight] : graph.getAdjList()[u]) {
            if (u < v) {
                edges.emplace_back(weight, u, v);
            }
        }
    }

    std::sort(edges.begin(), edges.end());
    UnionFind uf(graph.getNumVertices());

    int edgeCount = 0;
    for (const auto& [weight, u, v] : edges) {
        if (uf.unionSets(u, v)) {
            mst.add_edge(u, v, weight);
            edgeCount++;
        }
    }

    if (edgeCount < graph.getNumVertices() - 1) {
        return Graph(0); // No MST found
    }

    return mst;
}

// Integer MST Solver
Graph IntegerMSTSolver::solveMST(Graph& graph) {
    int V = graph.getNumVertices();
    Graph mst(V);

    std::vector<bool> inMST(V, false);
    std::vector<int> key(V, std::numeric_limits<int>::max());
    std::vector<int> parent(V, -1);

    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<>> pq;

    key[0] = 0;
    pq.push({0, 0});

    int edgeCount = 0;
    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();

        if (inMST[u]) continue;
        inMST[u] = true;

        for (const auto& [v, weight] : graph.getAdjList()[u]) {
            if (!inMST[v] && weight < key[v]) {
                key[v] = weight;
                pq.push({key[v], v});
                parent[v] = u;
            }
        }
    }

    for (int v = 1; v < V; ++v) {
        if (parent[v] != -1) {
            mst.add_edge(parent[v], v, key[v]);
            edgeCount++;
        }
    }

    if (edgeCount < V - 1) {
        return Graph(0); // No MST found
    }

    return mst;
}


// Constructor
UnionFind::UnionFind(int n) : parent(n), rank(n, 0) {
    for (int i = 0; i < n; ++i) {
        parent[i] = i;
    }
}

// Destructor
UnionFind::~UnionFind() = default;

// Find with path compression
int UnionFind::find(int u) {
    if (u != parent[u]) {
        parent[u] = find(parent[u]);
    }
    return parent[u];
}

// Union by rank
bool UnionFind::unionSets(int u, int v) {
    int rootU = find(u);
    int rootV = find(v);
    if (rootU != rootV) {
        if (rank[rootU] > rank[rootV]) {
            parent[rootV] = rootU;
        } else if (rank[rootU] < rank[rootV]) {
            parent[rootU] = rootV;
        } else {
            parent[rootV] = rootU;
            ++rank[rootU];
        }
        return true;
    }
    return false;
}