#ifndef ROUTEX_ALGORITHMS_HPP
#define ROUTEX_ALGORITHMS_HPP

#include "Graph.hpp"
#include <queue>
#include <vector>
#include <limits>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <iomanip>

/**
 * ============================================================================
 * STRUCT: DijkstraResult
 * ============================================================================
 * Encapsulates the complete result of Dijkstra's algorithm.
 */
struct DijkstraResult {
    bool found;                         // True if destination is reachable
    double totalDistance;               // Shortest distance in km
    std::vector<int> path;              // Sequence of vertex IDs from src to dest
    double executionTimeMicroseconds;   // Measured runtime in microseconds
    int nodesVisited;                   // Number of vertices finalized/popped
    int edgesRelaxed;                   // Number of successful edge relaxations
};

/**
 * ============================================================================
 * STRUCT: BFSResult
 * ============================================================================
 * Encapsulates the complete result of Breadth-First Search (BFS).
 */
struct BFSResult {
    bool found;                         // True if destination is reachable
    int hopCount;                       // Minimum number of edges/hops
    double totalDistance;               // Cumulative distance along this BFS path
    std::vector<int> path;              // Sequence of vertex IDs from src to dest
    double executionTimeMicroseconds;   // Measured runtime in microseconds
    int nodesVisited;                   // Number of vertices dequeued
};

/**
 * ============================================================================
 * CLASS: PathfindingAlgorithms
 * ============================================================================
 * Contains core algorithm implementations: Dijkstra and Breadth-First Search.
 */
class PathfindingAlgorithms {
public:
    static constexpr double INF = std::numeric_limits<double>::infinity();

    /**
     * ========================================================================
     * ALGORITHM: Dijkstra's Shortest Path Algorithm
     * ========================================================================
     * Objective:
     * Find the path with the minimum cumulative edge weight from source to destination.
     * 
     * Data Structures:
     * - Min-Priority Queue: std::priority_queue<std::pair<double, int>,
     *                                           std::vector<std::pair<double, int>>,
     *                                           std::greater<std::pair<double, int>>>
     *   Stores pairs of (tentative_distance, vertex_id).
     *   Extract-Min operation takes O(log V) time using a binary heap.
     * - dist[]: Vector storing the currently known shortest distance to each vertex.
     * - parent[]: Vector storing predecessor pointers for path reconstruction.
     * - visited[]: Tracks finalized vertices to ignore obsolete entries in the priority queue.
     * 
     * Time Complexity:  O((V + E) * log V) with binary heap
     * Space Complexity: O(V) auxiliary memory for dist, parent, visited, and heap.
     */
    /**
     * Core Dijkstra execution without timing wrapper.
     */
    static DijkstraResult runDijkstraCore(const Graph& graph, int src, int dest) {
        DijkstraResult res;
        res.found = false;
        res.totalDistance = INF;
        res.nodesVisited = 0;
        res.edgesRelaxed = 0;
        res.executionTimeMicroseconds = 0.0;

        int V = graph.getNumVertices();
        if (!graph.isValidVertex(src) || !graph.isValidVertex(dest)) {
            return res;
        }

        if (src == dest) {
            res.found = true;
            res.totalDistance = 0.0;
            res.path = {src};
            res.nodesVisited = 1;
            return res;
        }

        std::vector<double> dist(V, INF);
        std::vector<int> parent(V, -1);
        std::vector<bool> visited(V, false);

        using DistPair = std::pair<double, int>;
        std::priority_queue<DistPair, std::vector<DistPair>, std::greater<DistPair>> minHeap;

        dist[src] = 0.0;
        minHeap.push({0.0, src});

        while (!minHeap.empty()) {
            auto topElem = minHeap.top();
            minHeap.pop();
            double currDist = topElem.first;
            int u = topElem.second;

            if (currDist > dist[u]) continue;
            if (visited[u]) continue;
            visited[u] = true;
            res.nodesVisited++;

            if (u == dest) break;

            for (const auto& edge : graph.getNeighbors(u)) {
                int v = edge.to;
                double weight = edge.weight;

                if (!visited[v] && dist[u] + weight < dist[v]) {
                    dist[v] = dist[u] + weight;
                    parent[v] = u;
                    minHeap.push({dist[v], v});
                    res.edgesRelaxed++;
                }
            }
        }

        if (dist[dest] < INF) {
            res.found = true;
            res.totalDistance = dist[dest];

            int curr = dest;
            while (curr != -1) {
                res.path.push_back(curr);
                curr = parent[curr];
            }
            std::reverse(res.path.begin(), res.path.end());
        }

        return res;
    }

    /**
     * Timed Dijkstra call using std::chrono::steady_clock.
     */
    static DijkstraResult runDijkstra(const Graph& graph, int src, int dest) {
        auto startTime = std::chrono::steady_clock::now();
        DijkstraResult res = runDijkstraCore(graph, src, dest);
        auto endTime = std::chrono::steady_clock::now();

        std::chrono::duration<double, std::micro> elapsed = endTime - startTime;
        if (elapsed.count() > 0.01) {
            res.executionTimeMicroseconds = elapsed.count();
        } else {
            // For sub-microsecond single runs, run 1000 quick iterations of pure core algorithm
            const int CALIBRATION_RUNS = 1000;
            auto bStart = std::chrono::steady_clock::now();
            for (int i = 0; i < CALIBRATION_RUNS; ++i) {
                runDijkstraCore(graph, src, dest);
            }
            auto bEnd = std::chrono::steady_clock::now();
            std::chrono::duration<double, std::micro> bElapsed = bEnd - bStart;
            res.executionTimeMicroseconds = bElapsed.count() / CALIBRATION_RUNS;
        }

        return res;
    }

    /**
     * Core BFS execution without timing wrapper.
     */
    static BFSResult runBFSCore(const Graph& graph, int src, int dest) {
        BFSResult res;
        res.found = false;
        res.hopCount = 0;
        res.totalDistance = 0.0;
        res.nodesVisited = 0;
        res.executionTimeMicroseconds = 0.0;

        int V = graph.getNumVertices();
        if (!graph.isValidVertex(src) || !graph.isValidVertex(dest)) {
            return res;
        }

        if (src == dest) {
            res.found = true;
            res.path = {src};
            res.hopCount = 0;
            res.totalDistance = 0.0;
            res.nodesVisited = 1;
            return res;
        }

        std::vector<bool> visited(V, false);
        std::vector<int> parent(V, -1);
        std::vector<int> hops(V, -1);
        std::queue<int> q;

        visited[src] = true;
        hops[src] = 0;
        q.push(src);

        bool reached = false;

        while (!q.empty()) {
            int u = q.front();
            q.pop();
            res.nodesVisited++;

            if (u == dest) {
                reached = true;
                break;
            }

            for (const auto& edge : graph.getNeighbors(u)) {
                int v = edge.to;
                if (!visited[v]) {
                    visited[v] = true;
                    parent[v] = u;
                    hops[v] = hops[u] + 1;
                    q.push(v);
                }
            }
        }

        if (reached) {
            res.found = true;
            res.hopCount = hops[dest];

            int curr = dest;
            while (curr != -1) {
                res.path.push_back(curr);
                curr = parent[curr];
            }
            std::reverse(res.path.begin(), res.path.end());

            double roadDistance = 0.0;
            for (size_t i = 0; i + 1 < res.path.size(); ++i) {
                double w = graph.getEdgeWeight(res.path[i], res.path[i + 1]);
                if (w >= 0.0) roadDistance += w;
            }
            res.totalDistance = roadDistance;
        }

        return res;
    }

    /**
     * Timed BFS call using std::chrono::steady_clock.
     */
    static BFSResult runBFS(const Graph& graph, int src, int dest) {
        auto startTime = std::chrono::steady_clock::now();
        BFSResult res = runBFSCore(graph, src, dest);
        auto endTime = std::chrono::steady_clock::now();

        std::chrono::duration<double, std::micro> elapsed = endTime - startTime;
        if (elapsed.count() > 0.01) {
            res.executionTimeMicroseconds = elapsed.count();
        } else {
            const int CALIBRATION_RUNS = 1000;
            auto bStart = std::chrono::steady_clock::now();
            for (int i = 0; i < CALIBRATION_RUNS; ++i) {
                runBFSCore(graph, src, dest);
            }
            auto bEnd = std::chrono::steady_clock::now();
            std::chrono::duration<double, std::micro> bElapsed = bEnd - bStart;
            res.executionTimeMicroseconds = bElapsed.count() / CALIBRATION_RUNS;
        }

        return res;
    }

    /**
     * Formats and prints a reconstructed path with arrow notation.
     */
    static void printPath(const Graph& graph, const std::vector<int>& path) {
        if (path.empty()) {
            std::cout << "[None]\n";
            return;
        }
        for (size_t i = 0; i < path.size(); ++i) {
            std::cout << graph.getVertexName(path[i]);
            if (i + 1 < path.size()) {
                std::cout << " -> ";
            }
        }
        std::cout << "\n";
    }

    /**
     * Compares Dijkstra and BFS side-by-side with full pedagogical explanation.
     */
    static void displayComparison(const Graph& graph, int src, int dest) {
        std::cout << "\n=======================================================\n";
        std::cout << "      ALGORITHM COMPARISON: DIJKSTRA VS BFS            \n";
        std::cout << "=======================================================\n";
        std::cout << "Source:      " << graph.getVertexName(src) << " (ID: " << src << ")\n";
        std::cout << "Destination: " << graph.getVertexName(dest) << " (ID: " << dest << ")\n";
        std::cout << "-------------------------------------------------------\n";

        DijkstraResult dijkstra = runDijkstra(graph, src, dest);
        BFSResult bfs = runBFS(graph, src, dest);

        if (!dijkstra.found || !bfs.found) {
            std::cout << "\n[Notice] No path exists between " << graph.getVertexName(src)
                      << " and " << graph.getVertexName(dest) << ".\n";
            std::cout << "Both algorithms correctly identified the destination as unreachable!\n";
            std::cout << "=======================================================\n\n";
            return;
        }

        std::cout << "\n[1] DIJKSTRA'S ALGORITHM (Weighted Shortest Path)\n";
        std::cout << "    Route: ";
        printPath(graph, dijkstra.path);
        std::cout << "    Total Distance:   " << std::fixed << std::setprecision(2) << dijkstra.totalDistance << " km\n";
        std::cout << "    Total Hops/Edges: " << (dijkstra.path.size() - 1) << " hops\n";
        std::cout << "    Nodes Visited:    " << dijkstra.nodesVisited << "\n";
        std::cout << "    Edges Relaxed:    " << dijkstra.edgesRelaxed << "\n";
        std::cout << "    Execution Time:   " << std::fixed << std::setprecision(3) << dijkstra.executionTimeMicroseconds << " microseconds\n";

        std::cout << "\n[2] BREADTH-FIRST SEARCH (BFS - Unweighted Fewest Hops)\n";
        std::cout << "    Route: ";
        printPath(graph, bfs.path);
        std::cout << "    Total Distance:   " << std::fixed << std::setprecision(2) << bfs.totalDistance << " km (actual road distance along BFS path)\n";
        std::cout << "    Total Hops/Edges: " << bfs.hopCount << " hops (MINIMAL edges)\n";
        std::cout << "    Nodes Visited:    " << bfs.nodesVisited << "\n";
        std::cout << "    Execution Time:   " << std::fixed << std::setprecision(3) << bfs.executionTimeMicroseconds << " microseconds\n";

        std::cout << "\n-----------------------------------------------------------------\n";
        std::cout << "                   METRIC COMPARISON SUMMARY                     \n";
        std::cout << "-----------------------------------------------------------------\n";
        std::cout << std::left << std::setw(22) << "Metric"
                  << std::setw(24) << "Dijkstra"
                  << std::setw(22) << "BFS" << "\n";
        std::cout << "-----------------------------------------------------------------\n";
        std::cout << std::left << std::setw(22) << "Optimizes For"
                  << std::setw(24) << "Min Total Distance"
                  << std::setw(22) << "Min Edge/Hop Count" << "\n";
        std::cout << std::left << std::setw(22) << "Total Distance"
                  << std::setw(24) << (std::to_string(dijkstra.totalDistance).substr(0, 5) + " km")
                  << std::setw(22) << (std::to_string(bfs.totalDistance).substr(0, 5) + " km") << "\n";
        std::cout << std::left << std::setw(22) << "Total Hops (Edges)"
                  << std::setw(24) << (dijkstra.path.size() - 1)
                  << std::setw(22) << bfs.hopCount << "\n";
        std::cout << std::left << std::setw(22) << "Theoretical Time"
                  << std::setw(24) << "O((V + E) log V)"
                  << std::setw(22) << "O(V + E)" << "\n";
        std::cout << std::left << std::setw(22) << "Auxiliary Space"
                  << std::setw(24) << "O(V)"
                  << std::setw(22) << "O(V)" << "\n";
        std::cout << "-----------------------------------------------------------------\n";

        std::cout << "\n[CRITICAL DSA INSIGHT]:\n";
        if (dijkstra.totalDistance < bfs.totalDistance) {
            std::cout << "* Notice: Dijkstra found a physically SHORTER route ("
                      << dijkstra.totalDistance << " km vs " << bfs.totalDistance << " km),\n"
                      << "  even though it required " << (dijkstra.path.size() - 1) << " hops compared to BFS's "
                      << bfs.hopCount << " hops.\n"
                      << "* Why? BFS treats every road segment as identical (weight = 1).\n"
                      << "  Dijkstra accounts for true edge weights (actual kilometers).\n";
        } else if (dijkstra.totalDistance == bfs.totalDistance && (dijkstra.path.size() - 1) == static_cast<size_t>(bfs.hopCount)) {
            std::cout << "* In this specific pair, the path with the fewest hops ALSO happens to have the shortest distance.\n"
                      << "  Dijkstra and BFS chose equivalent routes.\n";
        } else {
            std::cout << "* Both algorithms found valid paths matching their respective objectives.\n";
        }
        std::cout << "=======================================================\n\n";
    }
};

#endif // ROUTEX_ALGORITHMS_HPP
