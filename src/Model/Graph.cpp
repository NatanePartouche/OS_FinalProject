#include "Graph.hpp"
#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>
#include <functional>
#include <sstream>

// Constructor to initialize a graph with a specified number of vertices.
Graph::Graph(int vertices) {
    adjList.resize(vertices);
}
Graph::~Graph() {
    // Libération des ressources (optionnel dans ce cas)
    for (auto& edges : adjList) {
        edges.clear();
    }
    adjList.clear();
}

// Adds an undirected edge between vertices `u` and `v` with a specified weight.
void Graph::add_edge_on_Graph(int u, int v, int weight) {
    if (isValidVertex(u) && isValidVertex(v)) {
        adjList[u].push_back({v, weight});
        adjList[v].push_back({u, weight});
    }
}

// Removes an undirected edge between vertices `u` and `v`.
void Graph::remove_edge_on_Graph(int u, int v) {
    if (isValidVertex(u) && isValidVertex(v)) {
        auto& neighborsU = adjList[u];
        for (auto it = neighborsU.begin(); it != neighborsU.end(); ++it) {
            if (it->first == v) {
                neighborsU.erase(it);
                break;
            }
        }

        auto& neighborsV = adjList[v];
        for (auto it = neighborsV.begin(); it != neighborsV.end(); ++it) {
            if (it->first == u) {
                neighborsV.erase(it);
                break;
            }
        }
    }
}

// Changes the weight of an existing undirected edge between vertices `u` and `v` to `newWeight`.
void Graph::changeEdgeWeight(int u, int v, int newWeight) {
    if (isValidVertex(u) && isValidVertex(v)) {
        for (auto& neighbor : adjList[u]) {
            if (neighbor.first == v) {
                neighbor.second = newWeight;
            }
        }
        for (auto& neighbor : adjList[v]) {
            if (neighbor.first == u) {
                neighbor.second = newWeight;
            }
        }
    }
}

// Provides a textual representation of the graph, showing all vertices and edges with weights.
std::string Graph::displayGraph() {
    std::string graphRepresentation;
    graphRepresentation += "---------------Graph Representation--------------------\n";
    graphRepresentation += "Vertices in the graph: ";
    for (int i = 0; i < getNumVertices(); ++i) {
        graphRepresentation += std::to_string(i) + " ";
    }
    graphRepresentation += "\nConnections between vertices (undirected edges):\n";
    for (int i = 0; i < getNumVertices(); ++i) {
        for (const auto& neighbor : adjList[i]) {
            if (i < neighbor.first) {
                graphRepresentation += "Vertex " + std::to_string(i) + " <----(" + std::to_string(neighbor.second) + ")----> Vertex " + std::to_string(neighbor.first) + "\n";
            }
        }
    }
    graphRepresentation += "-------------------------------------------------------\n";
    return graphRepresentation;
}

// Provides a textual representation of MST
std::string Graph::displayMST() {
    std::string graphRepresentation;
    graphRepresentation += "---------------MST Representation----------------------\n";
    graphRepresentation += "Vertices in the graph: ";
    for (int i = 0; i < getNumVertices(); ++i) {
        graphRepresentation += std::to_string(i) + " ";
    }
    graphRepresentation += "\nConnections between vertices (undirected edges):\n";
    for (int i = 0; i < getNumVertices(); ++i) {
        for (const auto& neighbor : adjList[i]) {
            if (i < neighbor.first) {
                graphRepresentation += "Vertex " + std::to_string(i) + " <----(" + std::to_string(neighbor.second) + ")----> Vertex " + std::to_string(neighbor.first) + "\n";
            }
        }
    }
    graphRepresentation += "-------------------------------------------------------\n";
    return graphRepresentation;
}

// Returns the total number of vertices in the graph.
int Graph::getNumVertices() {
    return static_cast<int>(adjList.size());
}

// Returns a constant reference to the adjacency list for accessing the graph structure externally.
const std::vector<std::list<std::pair<int, int>>>& Graph::getAdjList() {
    return adjList;
}

// Checks if a given vertex `v` is valid by ensuring it is within the range of defined vertices.
bool Graph::isValidVertex(int v) const {
    return v >= 0 && v < static_cast<int>(adjList.size());
}

// Compares this graph with another graph to check if they have the same structure and weights.
bool Graph::compareGraphs(Graph& other) {
    if (this->getNumVertices() != other.getNumVertices()) return false;
    if (this->getTotalWeight() != other.getTotalWeight()) return false;

    for (int i = 0; i < getNumVertices(); ++i) {
        const auto& thisNeighbors = this->adjList[i];
        const auto& otherNeighbors = other.adjList[i];

        // Check if each vertex has the same number of neighbors.
        if (thisNeighbors.size() != otherNeighbors.size()) return false;

        // Convert neighbors to sorted vectors for direct comparison.
        std::vector<std::pair<int, int>> sortedThisNeighbors(thisNeighbors.begin(), thisNeighbors.end());
        std::vector<std::pair<int, int>> sortedOtherNeighbors(otherNeighbors.begin(), otherNeighbors.end());

        std::sort(sortedThisNeighbors.begin(), sortedThisNeighbors.end());
        std::sort(sortedOtherNeighbors.begin(), sortedOtherNeighbors.end());

        // Compare sorted lists of edges for equality.
        if (sortedThisNeighbors != sortedOtherNeighbors) return false;
    }
    return true;
}

// Assignment operator
Graph& Graph::operator=(const Graph& other) {
    if (this != &other) {
        adjList = other.adjList;
    }
    return *this;
}

// Returns the total weight of all edges in the graph.
int Graph::getTotalWeight() {
    int totalWeight = 0;
    for (int u = 0; u < getNumVertices(); ++u) {
        for (const auto& neighbor : adjList[u]) {
            totalWeight += neighbor.second;  // Add the edge weight.
        }
    }
    return totalWeight / 2;
}

// Finds the longest path in the MST and returns it as a formatted string.
std::string Graph::getTreeDepthPath_MST() {
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

// Retrieves the heaviest edge in the MST as a formatted string "u v w".
std::string Graph::getMaxWeightEdge_MST() {
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

// Finds the heaviest path in the MST and returns it as a formatted string.
std::string Graph::getMaxWeightPath_MST() {
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

// Calculates the average distance between all vertex pairs in the MST.
double Graph::getAverageDistance_MST() {
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

// Retrieves the lightest edge in the MST as a formatted string "Vertex u <----(w)----> Vertex v".
std::string Graph::getMinWeightEdge_MST() {
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

// Function to find the path between two vertices in an MST without calling a helper function
std::string Graph::findPath_MST(int u, int v) {
    std::vector<bool> visited(this->getNumVertices(), false); // Track visited vertices
    std::vector<int> path; // Store the path

    // Direct DFS logic to find the path
    path.push_back(u);
    visited[u] = true;

    while (!path.empty()) {
        int current = path.back();

        if (current == v) {
            // Path found, build the path string
            std::ostringstream oss;
            for (size_t i = 0; i < path.size(); ++i) {
                oss << path[i];
                if (i < path.size() - 1) oss << "->";
            }
            return oss.str();
        }

        bool foundUnvisitedNeighbor = false;
        for (const auto& neighbor : adjList[current]) {
            int nextVertex = neighbor.first;
            if (!visited[nextVertex]) {
                visited[nextVertex] = true;
                path.push_back(nextVertex);
                foundUnvisitedNeighbor = true;
                break;
            }
        }
        // Backtrack if no unvisited neighbors
        if (!foundUnvisitedNeighbor) {
            path.pop_back();
        }
    }

    // If no path was found
    return "No path found between vertices " + std::to_string(u) + " and " + std::to_string(v);
}