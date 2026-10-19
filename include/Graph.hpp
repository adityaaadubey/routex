#ifndef ROUTEX_GRAPH_HPP
#define ROUTEX_GRAPH_HPP

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <iomanip>
#include <algorithm>
#include <cctype>

/**
 * ============================================================================
 * STRUCT: Edge
 * ============================================================================
 * Represents a directed or undirected connection from one vertex to another.
 * 
 * DSA Concept:
 * In a weighted graph, an edge connects vertex u to vertex v with an associated
 * numerical weight representing cost, distance, latency, or time.
 */
struct Edge {
    int to;          // Target vertex index
    double weight;   // Distance/cost along this edge (e.g. in kilometers)
};

/**
 * ============================================================================
 * CLASS: Graph
 * ============================================================================
 * Represents a weighted road network using an Adjacency List.
 * 
 * DSA Concepts & Trade-offs:
 * 1. Representation: Adjacency List (vector of vectors of Edge).
 *    - Space Complexity: O(V + E) where V is vertices, E is edges.
 *    - Ideal for sparse networks like real-world road networks where each
 *      intersection connects to only a few neighbors (average degree ~ 3-5).
 *    - Comparison with Adjacency Matrix: Adjacency Matrix takes O(V^2) memory
 *      and O(V) to iterate over neighbors of a vertex, whereas Adjacency List
 *      takes O(deg(u)) to explore neighbors.
 * 2. Name-to-Index Mapping:
 *    - Employs std::unordered_map<string, int> for O(1) average lookup.
 *    - Stores location names and descriptions for display purposes.
 */
class Graph {
private:
    int numVertices;
    int numEdges;
    std::vector<std::vector<Edge>> adjList;
    std::vector<std::string> vertexNames;
    std::vector<std::string> vertexDescriptions;
    std::unordered_map<std::string, int> nameToIndex;

    // Helper to normalize strings for case-insensitive lookup
    static std::string toLower(const std::string& str) {
        std::string lowerStr = str;
        std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        return lowerStr;
    }

public:
    explicit Graph(int vertices = 0)
        : numVertices(vertices), numEdges(0) {
        adjList.resize(vertices);
        vertexNames.resize(vertices);
        vertexDescriptions.resize(vertices);
    }

    // Add a new named vertex
    int addVertex(const std::string& name, const std::string& description = "") {
        int id = numVertices++;
        adjList.emplace_back();
        vertexNames.push_back(name);
        vertexDescriptions.push_back(description);
        nameToIndex[toLower(name)] = id;
        return id;
    }

    // Add an edge between u and v
    void addEdge(int u, int v, double weight, bool bidirectional = true) {
        if (u < 0 || u >= numVertices || v < 0 || v >= numVertices) {
            std::cerr << "[Error] Invalid vertex index for edge (" << u << ", " << v << ")\n";
            return;
        }
        if (weight < 0.0) {
            std::cerr << "[Warning] Negative edge weights are not supported by Dijkstra's algorithm.\n";
        }

        adjList[u].push_back({v, weight});
        numEdges++;

        if (bidirectional && u != v) {
            adjList[v].push_back({u, weight});
            numEdges++;
        }
    }

    // Lookup vertex ID by name (case-insensitive)
    int getVertexIdByName(const std::string& name) const {
        std::string lowerName = toLower(name);
        auto it = nameToIndex.find(lowerName);
        if (it != nameToIndex.end()) {
            return it->second;
        }
        return -1; // Not found
    }

    // Getters
    int getNumVertices() const { return numVertices; }
    int getNumEdges() const { return numEdges; }

    const std::vector<Edge>& getNeighbors(int u) const {
        return adjList[u];
    }

    std::string getVertexName(int u) const {
        if (u >= 0 && u < numVertices) {
            return vertexNames[u];
        }
        return "Unknown";
    }

    std::string getVertexDescription(int u) const {
        if (u >= 0 && u < numVertices) {
            return vertexDescriptions[u];
        }
        return "";
    }

    // Retrieve distance between two direct neighbors, or -1.0 if not connected
    double getEdgeWeight(int u, int v) const {
        if (u < 0 || u >= numVertices || v < 0 || v >= numVertices) return -1.0;
        for (const auto& edge : adjList[u]) {
            if (edge.to == v) return edge.weight;
        }
        return -1.0;
    }

    // Check if an index is valid
    bool isValidVertex(int u) const {
        return u >= 0 && u < numVertices;
    }

    // Display all locations
    void displayLocations() const {
        std::cout << "\n=======================================================\n";
        std::cout << "                 AVAILABLE LOCATIONS                   \n";
        std::cout << "=======================================================\n";
        std::cout << std::left << std::setw(6) << "ID"
                  << std::setw(26) << "Location Name"
                  << "Description\n";
        std::cout << "-------------------------------------------------------\n";
        for (int i = 0; i < numVertices; ++i) {
            std::cout << std::left << std::setw(6) << i
                      << std::setw(26) << vertexNames[i]
                      << vertexDescriptions[i] << "\n";
        }
        std::cout << "=======================================================\n";
        std::cout << "Total Locations: " << numVertices << "\n\n";
    }

    // Display adjacency list connections
    void displayGraph() const {
        std::cout << "\n=======================================================\n";
        std::cout << "            GRAPH ADJACENCY LIST CONNECTIONS           \n";
        std::cout << "=======================================================\n";
        std::cout << "Structure: [Source] -> (Neighbor, Distance in km)\n";
        std::cout << "-------------------------------------------------------\n";
        for (int u = 0; u < numVertices; ++u) {
            std::cout << "[" << std::setw(2) << u << "] " << std::left << std::setw(22) << vertexNames[u] << " -> ";
            if (adjList[u].empty()) {
                std::cout << "(Isolated / No outgoing connections)\n";
                continue;
            }
            for (size_t i = 0; i < adjList[u].size(); ++i) {
                const auto& edge = adjList[u][i];
                std::cout << vertexNames[edge.to] << " (" << std::fixed << std::setprecision(1)
                          << edge.weight << " km)";
                if (i + 1 < adjList[u].size()) {
                    std::cout << ", ";
                }
            }
            std::cout << "\n";
        }
        std::cout << "=======================================================\n";
        std::cout << "Total Directed Edges: " << numEdges << " (in adjacency list)\n\n";
    }
};

#endif // ROUTEX_GRAPH_HPP
