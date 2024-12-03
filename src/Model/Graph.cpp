#include "Graph.hpp"
#include "MSTFactory.hpp"
#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>
#include <functional>
#include <sstream>
#include <memory>

// Constructor to initialize a graph with a specified number of vertices.
Graph::Graph(int vertices) : adjList(vertices) {}

// Copy constructor
Graph::Graph(const Graph& other)
    : adjList(other.adjList), _algorithmChoice(other._algorithmChoice) {
    if (other.mst) {
        mst = std::make_unique<Graph>(*other.mst);
    }
}

// Copy assignment operator
Graph& Graph::operator=(const Graph& other) {
    if (this != &other) {
        adjList = other.adjList;
        _algorithmChoice = other._algorithmChoice;
        mst = other.mst ? std::make_unique<Graph>(*other.mst) : nullptr;
    }
    return *this;
}

// Move assignment operator
Graph& Graph::operator=(Graph&& other) noexcept {
    if (this != &other) {
        adjList = std::move(other.adjList);
        _algorithmChoice = std::move(other._algorithmChoice);
        mst = std::move(other.mst);
    }
    return *this;
}

// Adds an undirected edge between vertices `u` and `v` with a specified weight.
// If an edge already exists, it updates the weight.
void Graph::add_edge(int u, int v, int weight) {
    if (isValidVertex(u) && isValidVertex(v)) {
        // Remove the existing edge from u to v, if it exists
        for (auto it = adjList[u].begin(); it != adjList[u].end(); ++it) {
            if (it->first == v) {
                adjList[u].erase(it);
                break;
            }
        }

        // Remove the existing edge from v to u, if it exists
        for (auto it = adjList[v].begin(); it != adjList[v].end(); ++it) {
            if (it->first == u) {
                adjList[v].erase(it);
                break;
            }
        }

        // Add the new edge with the updated weight
        adjList[u].push_back({v, weight});
        adjList[v].push_back({u, weight});
    }
}

// Removes an undirected edge between vertices `u` and `v`.
void Graph::remove_edge(int u, int v) {
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

        ///////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////

// Provides a textual representation of the graph, showing all vertices and edges with weights.
std::string Graph::displayGraph() {
    std::string graphRepresentation;
    graphRepresentation +="\n" + std::string(15, ' ') + "---------------Graph Representation--------------------\n";
    graphRepresentation += std::string(15, ' ') + "Vertices in the graph: ";
    for (int i = 0; i < getNumVertices(); ++i) {
        graphRepresentation += std::to_string(i) + " ";
    }
    graphRepresentation += "\n" + std::string(15, ' ') + "Connections between vertices (undirected edges):\n";
    for (int i = 0; i < getNumVertices(); ++i) {
        for (const auto& neighbor : adjList[i]) {
            if (i < neighbor.first) {
                graphRepresentation += std::string(15, ' ') + "Vertex " + std::to_string(i) + " <----(" + std::to_string(neighbor.second) + ")----> Vertex " + std::to_string(neighbor.first) + "\n";
            }
        }
    }
    return graphRepresentation;
}

// Provides a textual representation of MST
std::string Graph::displayMST() {
    std::string graphRepresentation;
    graphRepresentation += std::string(15, ' ') + "---------------MST Representation----------------------\n";
    graphRepresentation += std::string(15, ' ') + "Vertices in the graph: ";
    for (int i = 0; i < this->mst->getNumVertices(); ++i) {
        graphRepresentation += std::to_string(i) + " ";
    }
    graphRepresentation += "\n" + std::string(15, ' ') + "Connections between vertices (undirected edges):\n";
    for (int i = 0; i < this->mst->getNumVertices(); ++i) {
        for (const auto& neighbor : this->mst->adjList[i]) {
            if (i < neighbor.first) {
                graphRepresentation += std::string(15, ' ') + "Vertex " + std::to_string(i) + " <----(" + std::to_string(neighbor.second) + ")----> Vertex " + std::to_string(neighbor.first) + "\n";
            }
        }
    }
    return graphRepresentation;
}

// Returns the total weight of all edges in the graph.
double Graph::getTotalWeight() {
    double totalWeight = 0;
    for (int u = 0; u < getNumVertices(); ++u) {
        for (const auto& neighbor : adjList[u]) {
            totalWeight += neighbor.second;  // Add the edge weight.
        }
    }
    return totalWeight / 2;
}

// Returns the total weight of all edges in the graph.
double Graph::getTotalWeight_MST() {
    double totalWeight = 0;
    for (int u = 0; u < this->mst->getNumVertices(); ++u) {
        for (const auto& neighbor : this->mst->adjList[u]) {
            totalWeight += neighbor.second;  // Add the edge weight.
        }
    }
    return totalWeight / 2;
}

// Finds the longest path in the MST and returns it as a formatted string.
std::string Graph::getTreeDepthPath_MST() {
    int n = this->mst->getNumVertices();
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
            for (const auto& neighbor : this->mst->adjList[node]) {
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
    for (int i = 0; i < this->mst->getNumVertices(); ++i) {
        for (const auto& edge : this->mst->adjList[i]) {
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

    int n = this->mst->getNumVertices();
    if (n == 0) return "Empty graph";

    std::vector<int> parents(n, -1);

    // Lambda function to find the farthest node from a given starting point.
    auto farthestFrom = [&](int start) {
        std::vector<bool> visited(n, false);
        std::vector<int> distance(n, 0);
        int maxDistance = 0, farthestNode = start;

        std::function<void(int)> dfs = [&](int node) {
            visited[node] = true;
            for (const auto& neighbor : this->mst->adjList[node]) {
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
            auto it = std::find_if(this->mst->adjList[u].begin(), this->mst->adjList[u].end(),
                                   [v](const std::pair<int, int>& edge) { return edge.first == v; });
            if (it != this->mst->adjList[u].end()) {
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
    int n = this->mst->getNumVertices();
    std::vector<std::vector<int>> dist(n, std::vector<int>(n, std::numeric_limits<int>::max()));
    for (int i = 0; i < n; ++i) {
        dist[i][i] = 0;
        for (const auto& neighbor : this->mst->adjList[i]) {
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
    for (int i = 0; i < this->mst->getNumVertices(); ++i) {
        for (const auto& edge : this->mst->adjList[i]) {
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

std::string Graph::Analysis() {
    std::string _Analysis = "";
    _Analysis +="\n"+displayGraph() + displayMST();
    _Analysis += std::string(15, ' ') + "------------------MST Analysis-------------------------\n";
    _Analysis += std::string(15, ' ') + "Algorithm: " + _algorithmChoice + "\n";
    _Analysis += std::string(15, ' ') + "Total MST weight: " + std::to_string(getTotalWeight_MST()) + "\n";
    _Analysis += std::string(15, ' ') + "Average distance: " + std::to_string(getAverageDistance_MST()) + "\n";
    _Analysis += std::string(15, ' ') + "Longest path: " + getTreeDepthPath_MST() + "\n";
    _Analysis += std::string(15, ' ') + "Heaviest path: " + getMaxWeightPath_MST() + "\n";
    _Analysis += std::string(15, ' ') + "Heaviest edge: " + getMaxWeightEdge_MST() + "\n";
    _Analysis += std::string(15, ' ') + "Lightest edge: " + getMinWeightEdge_MST() + "\n";
    _Analysis += std::string(15, ' ') + "-------------------------------------------------------\n\n";
    return _Analysis;
}

void Graph::Solve() {
    if (this->getNumVertices() == 0) {return ;}
    std::unique_ptr<MSTFactory> algo;
    if (_algorithmChoice == "prim") algo = std::make_unique<PrimSolver>();
    else if (_algorithmChoice == "kruskal") algo = std::make_unique<KruskalSolver>();
    else if (_algorithmChoice == "boruvka") algo = std::make_unique<BoruvkaSolver>();
    else if (_algorithmChoice == "tarjan") algo = std::make_unique<TarjanSolver>();
    else if (_algorithmChoice == "integer_mst") algo = std::make_unique<IntegerMSTSolver>();
    if (!algo) {return;}
        this->mst = std::make_unique<Graph>(algo->solveMST(*this));
}