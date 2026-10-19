#include "../include/Graph.hpp"
#include "../include/Algorithms.hpp"
#include "../include/Benchmark.hpp"
#include <iostream>
#include <string>
#include <limits>
#include <sstream>

/**
 * ============================================================================
 * HELPER: Robust Input Parsing
 * ============================================================================
 * Prevents terminal freeze/infinite loops when non-integer input is entered.
 */
static void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

static int getMenuChoice(int minVal, int maxVal) {
    int choice;
    while (true) {
        std::cout << "Enter your choice (" << minVal << "-" << maxVal << "): ";
        if (std::cin >> choice) {
            clearInputBuffer();
            if (choice >= minVal && choice <= maxVal) {
                return choice;
            }
            std::cout << "[Error] Choice out of range. Please choose between "
                      << minVal << " and " << maxVal << ".\n";
        } else {
            std::cout << "[Error] Invalid input. Please enter a valid number.\n";
            clearInputBuffer();
        }
    }
}

/**
 * Prompts user for a location by ID or by name.
 */
static int promptForLocation(const Graph& graph, const std::string& role) {
    while (true) {
        std::cout << "Enter " << role << " (by ID 0-" << (graph.getNumVertices() - 1)
                  << " or Name, or 'list' to view options): ";
        std::string input;
        if (!std::getline(std::cin, input)) {
            clearInputBuffer();
            continue;
        }

        // Trim leading and trailing whitespace
        size_t first = input.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) {
            std::cout << "[Error] Input cannot be blank. Try again.\n";
            continue;
        }
        size_t last = input.find_last_not_of(" \t\r\n");
        input = input.substr(first, (last - first + 1));

        if (input == "list" || input == "LIST" || input == "help") {
            graph.displayLocations();
            continue;
        }

        // Check if input is an integer ID
        std::stringstream ss(input);
        int id;
        if (ss >> id && ss.eof()) {
            if (graph.isValidVertex(id)) {
                return id;
            } else {
                std::cout << "[Error] Vertex ID " << id << " is invalid. Valid IDs are 0 to "
                          << (graph.getNumVertices() - 1) << ".\n";
                continue;
            }
        }

        // Otherwise, attempt name lookup
        int foundId = graph.getVertexIdByName(input);
        if (foundId != -1) {
            return foundId;
        }

        std::cout << "[Error] Location '" << input << "' was not found in the network.\n";
        std::cout << "Tip: Type 'list' to see all available location names and IDs.\n";
    }
}

/**
 * Initializes the default fictional 15-location city road network.
 */
static Graph buildFictionalCityNetwork() {
    Graph g;

    // 15 Fictional Locations
    g.addVertex("College Gate", "Main campus entrance & public transit hub");
    g.addVertex("Innovation Hub", "Incubation center, research labs & maker space");
    g.addVertex("Tech Park", "Software engineering offices & co-working spaces");
    g.addVertex("Central Library", "Historic municipal archives & quiet study halls");
    g.addVertex("City Center", "Heart of downtown commercial & civic district");
    g.addVertex("Railway Station", "Inter-city railway junction & grand concourse");
    g.addVertex("North Mall", "Major shopping center, cinema & dining avenue");
    g.addVertex("River Bridge", "Cable-stayed scenic crossing over Blue River");
    g.addVertex("Sunset Point", "Scenic hilltop observation deck & nature trail");
    g.addVertex("Green Park", "Botanical conservatory & eco-friendly green belt");
    g.addVertex("Market Square", "Bustling traditional market & artisan pavilions");
    g.addVertex("District Hospital", "24/7 Multi-specialty trauma & medical center");
    g.addVertex("Old Town", "Historic colonial district with cobblestone avenues");
    g.addVertex("Metro Terminal", "Underground rapid-transit interchange");
    g.addVertex("Remote Satellite Campus", "Isolated ecological research outpost (No roads)");

    // Road Connections (bidirectional, weights in km)
    // 0: College Gate connections
    g.addEdge(0, 1, 2.5);   // College Gate <-> Innovation Hub
    g.addEdge(0, 3, 4.0);   // College Gate <-> Central Library
    g.addEdge(0, 9, 3.0);   // College Gate <-> Green Park

    // EXPRESS HIGHWAY: Direct from College Gate (0) to Railway Station (5)
    // 14.0 km direct (1 hop). Contrast this with 0 -> 3 -> 4 -> 5 which is only 10.0 km (3 hops)!
    // BFS chooses 0 -> 5 (1 hop, 14 km), whereas Dijkstra chooses 0 -> 3 -> 4 -> 5 (10 km)!
    g.addEdge(0, 5, 14.0);

    // 1: Innovation Hub connections
    g.addEdge(1, 2, 3.5);   // Innovation Hub <-> Tech Park
    g.addEdge(1, 4, 6.0);   // Innovation Hub <-> City Center

    // 2: Tech Park connections
    g.addEdge(2, 5, 5.0);   // Tech Park <-> Railway Station
    g.addEdge(2, 6, 4.2);   // Tech Park <-> North Mall

    // 3: Central Library connections
    g.addEdge(3, 4, 2.8);   // Central Library <-> City Center
    g.addEdge(3, 8, 5.5);   // Central Library <-> Sunset Point

    // 4: City Center connections
    g.addEdge(4, 5, 3.2);   // City Center <-> Railway Station
    g.addEdge(4, 10, 2.0);  // City Center <-> Market Square

    // 5: Railway Station connections
    g.addEdge(5, 13, 4.0);  // Railway Station <-> Metro Terminal

    // 6: North Mall connections
    g.addEdge(6, 7, 3.8);   // North Mall <-> River Bridge
    g.addEdge(6, 13, 6.5);  // North Mall <-> Metro Terminal

    // 7: River Bridge connections
    g.addEdge(7, 8, 4.5);   // River Bridge <-> Sunset Point
    g.addEdge(7, 12, 5.0);  // River Bridge <-> Old Town

    // 8: Sunset Point connections
    g.addEdge(8, 9, 3.5);   // Sunset Point <-> Green Park

    // 9: Green Park connections
    g.addEdge(9, 10, 4.0);  // Green Park <-> Market Square

    // 10: Market Square connections
    g.addEdge(10, 11, 2.5); // Market Square <-> District Hospital

    // 11: District Hospital connections
    g.addEdge(11, 12, 3.0); // District Hospital <-> Old Town

    // 12: Old Town connections
    g.addEdge(12, 13, 3.5); // Old Town <-> Metro Terminal

    // Note: Node 14 (Remote Satellite Campus) is intentionally disconnected
    // to test unreachable destination handling.

    return g;
}

/**
 * Core Feature 3: Find Shortest Route with Dijkstra
 */
static void handleFindShortestRoute(const Graph& graph) {
    std::cout << "\n========================================\n";
    std::cout << "         FIND SHORTEST ROUTE (DIJKSTRA) \n";
    std::cout << "========================================\n";

    int src = promptForLocation(graph, "Source Location");
    int dest = promptForLocation(graph, "Destination Location");

    std::cout << "\nFinding shortest route from '" << graph.getVertexName(src)
              << "' to '" << graph.getVertexName(dest) << "'...\n";

    DijkstraResult res = PathfindingAlgorithms::runDijkstra(graph, src, dest);

    std::cout << "\n----------------------------------------\n";
    std::cout << "Source: " << graph.getVertexName(src) << "\n";
    std::cout << "Destination: " << graph.getVertexName(dest) << "\n\n";

    if (!res.found) {
        std::cout << "Result: Destination is UNREACHABLE.\n";
        std::cout << "No route exists between these two locations in the network.\n";
        std::cout << "----------------------------------------\n\n";
        return;
    }

    std::cout << "Shortest Route:\n";
    PathfindingAlgorithms::printPath(graph, res.path);
    std::cout << "\n";
    std::cout << "Total Distance: " << std::fixed << std::setprecision(1) << res.totalDistance << " km\n";
    std::cout << "Total Stops:    " << (res.path.size() - 1) << " road segment(s)\n";
    std::cout << "Algorithm:      Dijkstra (with Min-Priority Queue)\n";
    std::cout << "Execution Time: " << std::fixed << std::setprecision(3) << res.executionTimeMicroseconds << " microseconds\n";
    std::cout << "Nodes Visited:  " << res.nodesVisited << "\n";
    std::cout << "Edges Relaxed:  " << res.edgesRelaxed << "\n";
    std::cout << "----------------------------------------\n\n";
}

/**
 * Core Feature 4: Compare Dijkstra and BFS
 */
static void handleCompareAlgorithms(const Graph& graph) {
    std::cout << "\n========================================\n";
    std::cout << "      COMPARE PATHFINDING ALGORITHMS    \n";
    std::cout << "========================================\n";

    int src = promptForLocation(graph, "Source Location");
    int dest = promptForLocation(graph, "Destination Location");

    PathfindingAlgorithms::displayComparison(graph, src, dest);
}

/**
 * Automated System Test Suite
 * Validates all 6 edge cases required by specification:
 * 1. Known shortest path with multiple routes
 * 2. Unreachable destination
 * 3. Invalid source/destination
 * 4. Single-vertex graph (src == dest)
 * 5. Zero-weight edges
 * 6. Express highway vs multi-hop route (BFS vs Dijkstra)
 */
static void runAutomatedTests(const Graph& cityGraph) {
    std::cout << "\n===============================================================================\n";
    std::cout << "                 ROUTEX - AUTOMATED SYSTEM TEST SUITE                          \n";
    std::cout << "===============================================================================\n";
    int passed = 0;
    int total = 6;

    // Test 1: Known shortest path between College Gate (0) and Railway Station (5)
    // Path 0 -> 3 -> 4 -> 5 has distance 4.0 + 2.8 + 3.2 = 10.0 km.
    // Express direct highway 0 -> 5 is 14.0 km.
    {
        std::cout << "[Test 1/6] Known Shortest Path with Multiple Routes (0 -> 5)... ";
        DijkstraResult res = PathfindingAlgorithms::runDijkstra(cityGraph, 0, 5);
        if (res.found && std::abs(res.totalDistance - 10.0) < 1e-4 && res.path.size() == 4) {
            std::cout << "PASSED (Distance: " << res.totalDistance << " km, Expected: 10.0 km)\n";
            passed++;
        } else {
            std::cout << "FAILED (Distance: " << res.totalDistance << " km)\n";
        }
    }

    // Test 2: Unreachable destination (College Gate 0 -> Remote Satellite Campus 14)
    {
        std::cout << "[Test 2/6] Unreachable Destination in Disconnected Component (0 -> 14)... ";
        DijkstraResult res = PathfindingAlgorithms::runDijkstra(cityGraph, 0, 14);
        if (!res.found && res.path.empty()) {
            std::cout << "PASSED (Correctly flagged destination as unreachable)\n";
            passed++;
        } else {
            std::cout << "FAILED (Erroneously claimed reachable)\n";
        }
    }

    // Test 3: Invalid source or destination IDs
    {
        std::cout << "[Test 3/6] Out-of-bounds / Invalid Location IDs (-1 -> 99)... ";
        DijkstraResult res = PathfindingAlgorithms::runDijkstra(cityGraph, -1, 99);
        if (!res.found && res.path.empty()) {
            std::cout << "PASSED (Gracefully handled out-of-range vertices without crash)\n";
            passed++;
        } else {
            std::cout << "FAILED\n";
        }
    }

    // Test 4: Single vertex / Same Source & Destination (0 -> 0)
    {
        std::cout << "[Test 4/6] Single Vertex / Source equals Destination (3 -> 3)... ";
        DijkstraResult res = PathfindingAlgorithms::runDijkstra(cityGraph, 3, 3);
        if (res.found && res.totalDistance == 0.0 && res.path.size() == 1 && res.path[0] == 3) {
            std::cout << "PASSED (Distance: 0.0 km, Path: [3])\n";
            passed++;
        } else {
            std::cout << "FAILED\n";
        }
    }

    // Test 5: Graph with Zero-Weight Edges
    {
        std::cout << "[Test 5/6] Graph with Zero-Weight Edges (Free Tram Connection)... ";
        Graph zeroGraph(3);
        zeroGraph.addVertex("A");
        zeroGraph.addVertex("B");
        zeroGraph.addVertex("C");
        zeroGraph.addEdge(0, 1, 0.0); // Cost: 0.0
        zeroGraph.addEdge(1, 2, 5.0); // Cost: 5.0
        zeroGraph.addEdge(0, 2, 8.0); // Cost: 8.0

        DijkstraResult res = PathfindingAlgorithms::runDijkstra(zeroGraph, 0, 2);
        if (res.found && std::abs(res.totalDistance - 5.0) < 1e-4 && res.path.size() == 3) {
            std::cout << "PASSED (Optimal distance: 5.0 km via 0-cost edge)\n";
            passed++;
        } else {
            std::cout << "FAILED (Distance: " << res.totalDistance << ")\n";
        }
    }

    // Test 6: BFS vs Dijkstra Trade-off Verification
    {
        std::cout << "[Test 6/6] BFS (Fewest Hops) vs Dijkstra (Shortest Distance)... ";
        DijkstraResult dRes = PathfindingAlgorithms::runDijkstra(cityGraph, 0, 5);
        BFSResult bRes = PathfindingAlgorithms::runBFS(cityGraph, 0, 5);

        // BFS should take 1 hop (14.0 km direct)
        // Dijkstra should take 3 hops (10.0 km total)
        if (bRes.hopCount == 1 && std::abs(bRes.totalDistance - 14.0) < 1e-4 &&
            dRes.totalDistance < bRes.totalDistance) {
            std::cout << "PASSED (BFS: 1 hop/14km | Dijkstra: 3 hops/10km)\n";
            passed++;
        } else {
            std::cout << "FAILED (BFS hops: " << bRes.hopCount << ", Dijkstra dist: " << dRes.totalDistance << ")\n";
        }
    }

    std::cout << "-------------------------------------------------------------------------------\n";
    std::cout << "TEST SUMMARY: " << passed << " / " << total << " tests passed ("
              << (passed * 100 / total) << "%)\n";
    std::cout << "===============================================================================\n\n";
}

int main() {
    std::ios_base::sync_with_stdio(true);

    // Initialize City Graph
    Graph cityGraph = buildFictionalCityNetwork();

    bool running = true;
    while (running) {
        std::cout << "========================================\n";
        std::cout << "          ROUTEX - ROUTE FINDER         \n";
        std::cout << "========================================\n";
        std::cout << "\n";
        std::cout << "1. Display Available Locations\n";
        std::cout << "2. Display Graph Connections\n";
        std::cout << "3. Find Shortest Route\n";
        std::cout << "4. Compare Pathfinding Algorithms\n";
        std::cout << "5. Run Performance Benchmark\n";
        std::cout << "6. Run System Test Suite\n";
        std::cout << "0. Exit\n";
        std::cout << "\n";
        std::cout << "========================================\n";

        int choice = getMenuChoice(0, 6);

        switch (choice) {
            case 1:
                cityGraph.displayLocations();
                break;
            case 2:
                cityGraph.displayGraph();
                break;
            case 3:
                handleFindShortestRoute(cityGraph);
                break;
            case 4:
                handleCompareAlgorithms(cityGraph);
                break;
            case 5:
                Benchmark::runDijkstraBenchmark(50);
                break;
            case 6:
                runAutomatedTests(cityGraph);
                break;
            case 0:
                std::cout << "\nThank you for using RouteX! Exiting application.\n";
                running = false;
                break;
            default:
                break;
        }

        if (running && choice != 0) {
            std::cout << "Press [Enter] to return to the main menu...";
            std::string dummy;
            std::getline(std::cin, dummy);
            std::cout << "\n";
        }
    }

    return 0;
}
