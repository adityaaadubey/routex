#ifndef ROUTEX_BENCHMARK_HPP
#define ROUTEX_BENCHMARK_HPP

#include "Graph.hpp"
#include "Algorithms.hpp"
#include <vector>
#include <random>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <string>

/**
 * ============================================================================
 * STRUCT: BenchmarkResult
 * ============================================================================
 * Holds the measured and theoretical performance metrics for a graph size.
 */
struct BenchmarkResult {
    int vertices;
    int edges;
    double density;
    std::string graphType;
    int iterations;
    double avgTimeMicroseconds;
    std::string theoreticalComplexity;
    bool isValid;
    double sampleDistance;
};

/**
 * ============================================================================
 * CLASS: Benchmark
 * ============================================================================
 * Generates synthetic benchmark graphs and executes timed stress tests on
 * Dijkstra's algorithm across varying input sizes (V ≈ 50, 100, 250, 500).
 */
class Benchmark {
public:
    /**
     * Generates a connected, sparse synthetic road network graph of size V.
     * Graph generation is executed outside the timed section.
     */
    static Graph generateSyntheticRoadNetwork(int V, int targetAvgDegree = 4, unsigned int seed = 42) {
        Graph g(V);
        std::mt19937 rng(seed);
        std::uniform_real_distribution<double> distDist(1.0, 25.0);
        std::uniform_int_distribution<int> nodeDist(0, V - 1);

        // 1. Ensure connectedness by building a spanning backbone
        for (int i = 1; i < V; ++i) {
            std::uniform_int_distribution<int> prevDist(0, i - 1);
            int parent = prevDist(rng);
            double weight = distDist(rng);
            g.addEdge(parent, i, weight, true);
        }

        // 2. Add additional random road connections to reach target degree
        int desiredEdges = (V * targetAvgDegree) / 2;
        int currentEdges = V - 1;

        int attempts = 0;
        int maxAttempts = desiredEdges * 5;

        while (currentEdges < desiredEdges && attempts < maxAttempts) {
            attempts++;
            int u = nodeDist(rng);
            int v = nodeDist(rng);
            if (u == v) continue;

            // Check if edge already exists
            if (g.getEdgeWeight(u, v) >= 0.0) continue;

            double weight = distDist(rng);
            g.addEdge(u, v, weight, true);
            currentEdges++;
        }

        return g;
    }

    /**
     * Verifies that the shortest path produced by Dijkstra is valid:
     * 1. Path is non-empty.
     * 2. Starts at src, ends at dest.
     * 3. Every adjacent pair in the path has a valid edge.
     * 4. Sum of edge weights equals the totalDistance reported.
     */
    static bool validatePath(const Graph& g, int src, int dest, const DijkstraResult& res) {
        if (!res.found) return false;
        if (res.path.empty()) return false;
        if (res.path.front() != src || res.path.back() != dest) return false;

        double sumDist = 0.0;
        for (size_t i = 0; i + 1 < res.path.size(); ++i) {
            double w = g.getEdgeWeight(res.path[i], res.path[i + 1]);
            if (w < 0.0) return false; // Edge does not exist
            sumDist += w;
        }

        // Check if reconstructed edge sum matches totalDistance within floating point epsilon
        return std::abs(sumDist - res.totalDistance) < 1e-4;
    }

    /**
     * Runs comprehensive benchmark for graph sizes ~ 50, 100, 250, 500.
     * All graph construction and terminal printing are excluded from timed intervals.
     */
    static void runDijkstraBenchmark(int baseIterations = 100) {
        std::vector<int> sizes = {50, 100, 250, 500};
        std::vector<BenchmarkResult> results;

        std::cout << "\n===============================================================================\n";
        std::cout << "          ROUTEX - DIJKSTRA ALGORITHM PERFORMANCE BENCHMARK                    \n";
        std::cout << "===============================================================================\n";
        std::cout << "Testing Dijkstra with std::priority_queue (Min-Heap)\n";
        std::cout << "Timing mechanism: std::chrono::steady_clock with adaptive sample calibration\n";
        std::cout << "Graph structure: Connected sparse road networks (Average vertex degree ~ 4)\n";
        std::cout << "NOTE: Terminal printing & graph generation are excluded from the timed section.\n";
        std::cout << "-------------------------------------------------------------------------------\n";
        std::cout << "Executing benchmarks... Please wait.\n";

        for (int V : sizes) {
            // STEP 1: Generate graph OUTSIDE the timed section
            Graph graph = generateSyntheticRoadNetwork(V, 4, 1000 + V);
            int E = graph.getNumEdges(); // Count of directed edges in adjacency list
            int undirectedEdges = E / 2;
            double maxEdges = static_cast<double>(V) * (V - 1) / 2.0;
            double density = (maxEdges > 0) ? (undirectedEdges / maxEdges) * 100.0 : 0.0;

            int src = 0;
            int dest = V - 1;

            // Warm-up run (ensures cache readiness, not included in statistics)
            DijkstraResult sampleRun = PathfindingAlgorithms::runDijkstra(graph, src, dest);
            bool valid = validatePath(graph, src, dest, sampleRun);

            // STEP 2: Timed loop using steady_clock
            // Dynamically scales runs so total measured interval comfortably exceeds
            // OS timer tick granularity on Windows (typically ~15ms).
            int effectiveIterations = baseIterations;
            double avgTime = 0.0;

            while (true) {
                auto startTotal = std::chrono::steady_clock::now();
                for (int i = 0; i < effectiveIterations; ++i) {
                    PathfindingAlgorithms::runDijkstraCore(graph, src, dest);
                }
                auto endTotal = std::chrono::steady_clock::now();
                std::chrono::duration<double, std::milli> elapsedMs = endTotal - startTotal;

                if (elapsedMs.count() >= 25.0 || effectiveIterations >= 10000) {
                    std::chrono::duration<double, std::micro> elapsedUs = endTotal - startTotal;
                    avgTime = elapsedUs.count() / static_cast<double>(effectiveIterations);
                    break;
                }
                effectiveIterations *= 2; // Increase sample count
            }

            BenchmarkResult res;
            res.vertices = V;
            res.edges = undirectedEdges;
            res.density = density;
            res.graphType = "Sparse Road Network";
            res.iterations = effectiveIterations;
            res.avgTimeMicroseconds = avgTime;
            res.theoreticalComplexity = "O((V + E) log V)";
            res.isValid = valid;
            res.sampleDistance = sampleRun.totalDistance;

            results.push_back(res);
        }

        // STEP 3: Display results in a clean formatted table
        std::cout << "\n===============================================================================\n";
        std::cout << "                           BENCHMARK RESULTS TABLE                             \n";
        std::cout << "===============================================================================\n";
        std::cout << std::left
                  << std::setw(10) << "Vertices"
                  << std::setw(10) << "Edges"
                  << std::setw(12) << "Density"
                  << std::setw(20) << "Avg Time (us)"
                  << std::setw(18) << "Complexity"
                  << std::setw(12) << "Path Valid?"
                  << "\n";
        std::cout << "-------------------------------------------------------------------------------\n";

        for (const auto& r : results) {
            std::string densityStr = std::to_string(r.density).substr(0, 4) + "%";
            std::string validStr = r.isValid ? "YES (Passed)" : "NO (Failed)";

            std::cout << std::left
                      << std::setw(10) << r.vertices
                      << std::setw(10) << r.edges
                      << std::setw(12) << densityStr
                      << std::setw(20) << std::fixed << std::setprecision(2) << r.avgTimeMicroseconds
                      << std::setw(18) << r.theoreticalComplexity
                      << std::setw(12) << validStr
                      << "\n";
        }
        std::cout << "===============================================================================\n";

        // Analysis and viva talking points
        std::cout << "\n[ACADEMIC PERFORMANCE ANALYSIS]:\n";
        std::cout << "1. Scaling Behavior:\n";
        std::cout << "   - For V = 50 to V = 500 (10x increase in vertices), execution time scales gracefully.\n";
        std::cout << "   - Because road networks are sparse (E = O(V)), Dijkstra's runtime simplifies to\n";
        std::cout << "     O(V log V), which is near-linear.\n";
        std::cout << "2. Priority Queue Efficiency:\n";
        std::cout << "   - Using std::priority_queue (Binary Min-Heap) provides O(log V) push and pop.\n";
        std::cout << "   - A naive linear scan array implementation would take O(V^2), which would be much\n";
        std::cout << "     slower for V >= 500.\n";
        std::cout << "3. Hardware & Real-World Variances:\n";
        std::cout << "   - Actual execution times depend on CPU architecture, cache locality (L1/L2 hits),\n";
        std::cout << "     compiler optimizations (-O2 vs -O0), and background OS scheduling.\n";
        std::cout << "   - Averaging over multiple benchmark runs minimizes single-sample jitter.\n";
        std::cout << "===============================================================================\n\n";
    }
};

#endif // ROUTEX_BENCHMARK_HPP
