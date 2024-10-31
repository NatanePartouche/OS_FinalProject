#include "MST.hpp"
#include "MSTFactory.hpp"
#include <limits>
#include <algorithm>
#include <vector>
#include <functional>
#include <sstream>

MST::MST() : Graph(0) {}

MST::MST(Graph& graph, std::string Type_of_Algo) : Graph(0) {
    // Instantiate the appropriate solver based on the specified algorithm type.
    MSTFactory* solver = nullptr;

    if (Type_of_Algo == "PRIM") {
        solver = new PrimSolver();
    } else if (Type_of_Algo == "KRUSKAL") {
        solver = new KruskalSolver();
    } else if (Type_of_Algo == "BORUVKA") {
        solver = new BoruvkaSolver();
    } else if (Type_of_Algo == "TARJAN") {
        solver = new TarjanSolver();
    } else if (Type_of_Algo == "INTEGER_MST") {
        solver = new IntegerMSTSolver();
    } else {
        std::cerr << "Unknown MST algorithm type!" << std::endl;
        return;
    }

    // Solve the MST using the chosen algorithm and update the adjacency list.
    Graph mstResult = solver->solveMST(graph);
    delete solver;

    this->adjList = mstResult.getAdjList();
}

// 1. Returns the total weight of the MST as a double.
double MST::getTotalWeight() {
    double totalWeight = 0.0;
    for (const auto& neighbors : adjList) {
        for (const auto& edge : neighbors) {
            totalWeight += edge.second;
        }
    }
    return totalWeight / 2.0; // Divide by 2 because each edge is counted twice in the undirected graph.
}

// 2. Finds the longest path in the MST and returns it as a formatted string.
std::string MST::getTreeDepthPath() {
    int n = getNumVertices();
    if (n == 0) return "";

    std::vector<int> path;
    std::vector<int> parents(n, -1); // Declare parents array to store traversal path.

    // Lambda function to find the farthest node from a given starting node.
    auto farthestFrom = [&](int start) {
        std::vector<bool> visited(n, false);
        std::vector<int> distance(n, 0);
        int maxDistance = 0, farthestNode = start;

        std::function<void(int)> dfs = [&](int node) {
            visited[node] = true;
            for (const auto& neighbor : adjList[node]) {
                int v = neighbor.first;
                if (!visited[v]) {
                    parents[v] = node;
                    distance[v] = distance[node] + 1;
                    dfs(v);
                    if (distance[v] > maxDistance) {
                        maxDistance = distance[v];
                        farthestNode = v;
                    }
                }
            }
        };
        dfs(start);

        // Build the path from the starting node to the farthest node.
        path.clear();
        for (int v = farthestNode; v != -1; v = parents[v])
            path.push_back(v);
        std::reverse(path.begin(), path.end());
        return maxDistance;
    };
    farthestFrom(0); // Find the deepest path.

    // Convert the path to a formatted string "0->9->..."
    std::ostringstream oss;
    for (size_t i = 0; i < path.size(); ++i) {
        oss << path[i];
        if (i < path.size() - 1) oss << "->";
    }
    return oss.str();
}

// 3. Retrieves the heaviest edge in the MST as a formatted string "u v w".
std::string MST::getMaxWeightEdge() {
    int maxWeightEdge = 0;
    int u = -1, v = -1;
    for (int i = 0; i < getNumVertices(); ++i) {
        for (const auto& edge : adjList[i]) {
            if (edge.second > maxWeightEdge) {
                maxWeightEdge = edge.second;
                u = i;
                v = edge.first;
            }
        }
    }
    std::ostringstream oss;
    oss << "Vertex " << u << " <----(" << maxWeightEdge << ")----> Vertex " << v;
    return oss.str();
}

// 4. Finds the heaviest path in the MST and returns it as a formatted string.
std::string MST::getMaxWeightPath() {
    int n = getNumVertices();
    if (n == 0) return "Empty graph";

    std::vector<int> parents(n, -1);

    // Lambda function to find the farthest node from a given starting point.
    auto farthestFrom = [&](int start) {
        std::vector<bool> visited(n, false);
        std::vector<int> distance(n, 0);
        int maxDistance = 0, farthestNode = start;

        std::function<void(int)> dfs = [&](int node) {
            visited[node] = true;
            for (const auto& neighbor : adjList[node]) {
                int v = neighbor.first;
                int weight = neighbor.second;
                if (!visited[v]) {
                    parents[v] = node;
                    distance[v] = distance[node] + weight;
                    dfs(v);
                    if (distance[v] > maxDistance) {
                        maxDistance = distance[v];
                        farthestNode = v;
                    }
                }
            }
        };
        dfs(start);
        return farthestNode;
    };

    // Find the two ends of the heaviest path.
    int start = farthestFrom(0);

    // Reset `parents` for the second search.
    std::fill(parents.begin(), parents.end(), -1);
    int end = farthestFrom(start);

    // Rebuild the path as a list of vertex-weight pairs.
    std::vector<std::pair<int, int>> maxPath;
    for (int v = end; v != -1; v = parents[v]) {
        if (parents[v] != -1) {
            int u = parents[v];
            auto it = std::find_if(adjList[u].begin(), adjList[u].end(),
                                   [v](const std::pair<int, int>& edge) { return edge.first == v; });
            if (it != adjList[u].end()) {
                maxPath.push_back({u, it->second});
            }
        }
    }
    std::reverse(maxPath.begin(), maxPath.end());

    // Build a formatted string representation of the heaviest path.
    std::ostringstream oss;
    oss << "Heaviest path: ";
    for (size_t i = 0; i < maxPath.size(); ++i) {
        oss << maxPath[i].first << " --(" << maxPath[i].second << ")--> ";
    }
    oss << end;

    return oss.str();
}

// 5. Calculates the average distance between all vertex pairs in the MST.
double MST::getAverageDistance() {
    int n = getNumVertices();
    std::vector<std::vector<int>> dist(n, std::vector<int>(n, std::numeric_limits<int>::max()));
    for (int i = 0; i < n; ++i) {
        dist[i][i] = 0;
        for (const auto& neighbor : adjList[i]) {
            int v = neighbor.first;
            int weight = neighbor.second;
            dist[i][v] = weight;
        }
    }

    // Use Floyd-Warshall algorithm to compute shortest distances between all pairs.
    for (int k = 0; k < n; ++k)
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                if (dist[i][k] < std::numeric_limits<int>::max() && dist[k][j] < std::numeric_limits<int>::max())
                    dist[i][j] = std::min(dist[i][j], dist[i][k] + dist[k][j]);

    // Calculate the average of the pairwise distances.
    int sumDistances = 0, count = 0;
    for (int i = 0; i < n; ++i)
        for (int j = i + 1; j < n; ++j)
            if (dist[i][j] < std::numeric_limits<int>::max()) {
                sumDistances += dist[i][j];
                ++count;
            }

    return count > 0 ? static_cast<double>(sumDistances) / count : 0.0;
}

// 6. Retrieves the lightest edge in the MST as a formatted string "Vertex u <----(w)----> Vertex v".
std::string MST::getMinWeightEdge() {
    int minWeightEdge = std::numeric_limits<int>::max();
    int u = -1, v = -1;
    for (int i = 0; i < getNumVertices(); ++i) {
        for (const auto& edge : adjList[i]) {
            if (edge.second < minWeightEdge) {
                minWeightEdge = edge.second;
                u = i;
                v = edge.first;
            }
        }
    }
    std::ostringstream oss;
    oss << "Vertex " << u << " <----(" << minWeightEdge << ")----> Vertex " << v;
    return oss.str();
}

// Adds an edge and checks if it can improve the MST.
bool MST::add(int u, int v, int weight) {
    // Check if vertices exist in the graph.
    if (u >= getNumVertices() || v >= getNumVertices()) return false;

    // Create a temporary graph with the same edges as `this`.
    Graph tempGraph(getNumVertices());
    for (int i = 0; i < getNumVertices(); ++i) {
        for (const auto& edge : adjList[i]) {
            if (i < edge.first) {
                tempGraph.addEdge(i, edge.first, edge.second);
            }
        }
    }

    // Add edge (u, v, weight) to the temporary graph.
    tempGraph.addEdge(u, v, weight);

    // Create a new MST from the temporary graph.
    MST newMST(tempGraph, "KRUSKAL");  // Or "PRIM" as desired

    // If the new MST is different, non-empty, and more optimal, replace `this` MST with the new one.
    if (!newMST.compareGraphs(*this) && newMST.getNumVertices() > 0 && newMST.getTotalWeight() < this->getTotalWeight()) {
        *this = newMST;
        return true;
    }
    return false;
}

// Removes an edge and ensures the graph remains a valid MST.
bool MST::remove(int u, int v) {
    // Check if the edge (u, v) exists.
    auto itU = std::find_if(adjList[u].begin(), adjList[u].end(),
                            [v](const std::pair<int, int>& edge) { return edge.first == v; });
    auto itV = std::find_if(adjList[v].begin(), adjList[v].end(),
                            [u](const std::pair<int, int>& edge) { return edge.first == u; });

    if (itU == adjList[u].end() || itV == adjList[v].end()) return false;

    // Create a temporary graph without the edge (u, v).
    Graph tempGraph(getNumVertices());
    for (int i = 0; i < getNumVertices(); ++i) {
        for (const auto& edge : adjList[i]) {
            if ((i == u && edge.first == v) || (i == v && edge.first == u)) {
                continue;
            }
            if (i < edge.first) {
                tempGraph.addEdge(i, edge.first, edge.second);
            }
        }
    }

    // Create a new MST from the temporary graph.
    MST newMST(tempGraph, "KRUSKAL");  // Or "PRIM" as desired

    // If the new MST is different, non-empty, and maintains connectivity, replace `this` MST with the new one.
    if (!newMST.compareGraphs(*this) && newMST.getNumVertices() > 0 && newMST.getTotalWeight() < this->getTotalWeight()) {
        *this = newMST;
        return true;
    }
    return false;
}