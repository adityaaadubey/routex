# RouteX: Shortest Path Finder & Algorithm Performance Analyzer

**RouteX** is a modular C++17 college mini project designed to demonstrate core Data Structures and Algorithms (DSA) through an interactive, offline route-planning and pathfinding analysis tool. 

The application models a realistic municipal road network of interconnected fictional landmarks, implements Dijkstra’s algorithm using a Min-Priority Queue, provides side-by-side comparative analysis with Breadth-First Search (BFS), and includes a microsecond-resolution benchmark suite measuring empirical performance across graph sizes up to 500 vertices.

---

## Table of Contents
1. [Problem Statement & Objectives](#1-problem-statement--objectives)
2. [Data Structures Used](#2-data-structures-used)
3. [Graph Representation: Adjacency List](#3-graph-representation-adjacency-list)
4. [Dijkstra's Algorithm & Priority Queues](#4-dijkstras-algorithm--priority-queues)
5. [BFS vs. Dijkstra: Fundamental Differences](#5-bfs-vs-dijkstra-fundamental-differences)
6. [Complexity Analysis](#6-complexity-analysis)
7. [Compilation & Execution (Windows / MinGW)](#7-compilation--execution-windows--mingw)
8. [Sample Input & Output](#8-sample-input--output)
9. [Performance Analysis & Benchmark Methodology](#9-performance-analysis--benchmark-methodology)
10. [Viva Examination Q&A (15 Questions)](#10-viva-examination-qa-15-questions)

---

## 1. Problem Statement & Objectives

### Problem Statement
Finding the shortest path between physical points of interest (intersections, transit hubs, commercial centers) is a fundamental problem in computer science and transportation engineering. While navigation engines typically rely on specialized variations, understanding the foundational mechanics of graph traversal, greedy edge relaxation, priority heaps, and complexity tradeoffs is critical for software engineering and computer science students.

### Project Objectives
1. **Model a Road Network:** Represent a weighted, undirected network of 15 fictional city locations using an efficient Adjacency List.
2. **Shortest Path Determination:** Implement Dijkstra’s Algorithm using a standard C++ Min-Priority Queue (`std::priority_queue`) to compute optimal paths and exact cumulative travel distances.
3. **Comparative Analysis:** Implement Breadth-First Search (BFS) to highlight the distinct computational goals of minimizing *edge counts* (hops) versus minimizing *cumulative edge weights* (kilometers).
4. **Performance Profiling:** Benchmark Dijkstra's algorithm across synthetic graphs of varying orders ($V = 50, 100, 250, 500$) using high-resolution monotonic clocks (`std::chrono::steady_clock`) to observe empirical scaling.
5. **Interactive & Resilient UI:** Provide a clean, robust, menu-driven terminal interface capable of handling invalid inputs, unreachable destinations, and self-testing without failure.

---

## 2. Data Structures Used

| Data Structure | C++ Standard Type | Role in Project | Key Operation Complexity |
| :--- | :--- | :--- | :--- |
| **Adjacency List** | `std::vector<std::vector<Edge>>` | Stores vertices and their incident directed/undirected edges | Neighbor iteration: $O(\text{deg}(u))$ |
| **Min-Priority Queue** | `std::priority_queue<Pair, vector<Pair>, greater<Pair>>` | Extracts the vertex with the lowest tentative distance in Dijkstra | Push: $O(\log V)$, Pop (Extract-Min): $O(\log V)$ |
| **Distance Array** | `std::vector<double>` | Maintains tentative shortest distances from source to all vertices | Lookup & Update: $O(1)$ |
| **Parent Array** | `std::vector<int>` | Stores predecessor pointers to reconstruct the optimal route | Pointer assignment: $O(1)$ |
| **FIFO Queue** | `std::queue<int>` | Layer-by-layer frontier exploration in BFS | Push & Pop: $O(1)$ |
| **Hash Map** | `std::unordered_map<string, int>` | Bidirectional lookup of location names to integer vertex IDs | Average lookup: $O(1)$ |

---

## 3. Graph Representation: Adjacency List

A graph $G = (V, E)$ consists of a set of vertices $V$ and edges $E$. In RouteX, each edge contains a target vertex index and a floating-point distance weight:

```cpp
struct Edge {
    int to;          // Neighbor vertex ID
    double weight;   // Road length in kilometers
};
```

### Why Adjacency List instead of Adjacency Matrix?
* **Memory Footprint:** 
  - Road networks are **sparse graphs** where each landmark connects only to a handful of neighboring intersections (average degree $\approx 3 \text{ to } 5$).
  - An **Adjacency Matrix** requires a $V \times V$ matrix ($O(V^2)$ space), wasting significant memory on zeros.
  - An **Adjacency List** requires only $O(V + E)$ space, storing exclusively existent road segments.
* **Traversal Efficiency:**
  - Finding the neighbors of vertex $u$ in an adjacency matrix requires scanning all $V$ entries in row $u$ ($O(V)$ time).
  - In an adjacency list, neighbor exploration takes only $O(\text{deg}(u))$ time, which speeds up Dijkstra and BFS significantly.

---

## 4. Dijkstra's Algorithm & Priority Queues

Dijkstra's Algorithm is a **greedy algorithm** that solves the Single-Source Shortest Path (SSSP) problem on graphs with non-negative edge weights.

```
                  +-----------------------------------+
                  |   Initialize dist[src] = 0        |
                  |   dist[all others] = INF          |
                  |   Push (0.0, src) to Min-Heap     |
                  +-----------------+-----------------+
                                    |
                                    v
                       +------------+-----------+
                       |   Is Min-Heap Empty?   | <----------------+
                       +------------+-----------+                  |
                                    | No                           |
                                    v                              |
                  +-----------------+-----------------+            |
                  | Pop (currDist, u) with minimum d  |            |
                  | If u == dest -> Reconstruct & End |            |
                  +-----------------+-----------------+            |
                                    |                              |
                                    v                              |
                     /-----------------------------\               |
                    <  Has vertex u been visited?   > --- Yes -----+ (Skip stale)
                     \-----------------------------/
                                    | No
                                    v
                  +-----------------+-----------------+
                  | Mark u as visited (finalized)     |
                  | For each neighbor (u -> v, w):    |
                  |   If dist[u] + w < dist[v]:       |
                  |      dist[v] = dist[u] + w        |
                  |      parent[v] = u                |
                  |      Push (dist[v], v) to Min-Heap|
                  +-----------------+-----------------+
                                    |
                                    +------------------------------+
```

### Role of the Min-Priority Queue
Without a priority queue, finding the unvisited vertex with the minimum distance requires scanning the entire distance vector, taking $O(V)$ time per vertex and leading to an overall runtime of $O(V^2)$. 

By structuring the priority queue as a **Binary Min-Heap** (`std::priority_queue` with `std::greater`), the minimum-distance vertex is extracted in $O(\log V)$ time.

---

## 5. BFS vs. Dijkstra: Fundamental Differences

A common misconception among beginner programmers is assuming that BFS and Dijkstra solve the exact same problem. They have fundamentally distinct objectives:

| Feature | Dijkstra's Algorithm | Breadth-First Search (BFS) |
| :--- | :--- | :--- |
| **Graph Type** | Weighted (non-negative weights) | Unweighted (or uniform weight = 1) |
| **Optimization Target** | **Minimum cumulative weight** (distance in km) | **Minimum number of edges** (hop count) |
| **Frontier Structure** | Min-Priority Queue (Binary Min-Heap) | FIFO Queue (`std::queue`) |
| **Time Complexity** | $O((V + E) \log V)$ | $O(V + E)$ |
| **Road Network Analogy** | "Find the route with the fewest kilometers" | "Find the route with the fewest road segments / turns" |

### Real-World Example in RouteX:
Consider traveling from **College Gate (0)** to **Railway Station (5)**:
- **Direct Express Highway:** Connects Gate (0) directly to Station (5) with a distance of **14.0 km**.
- **Local City Roads:** Gate (0) $\to$ Central Library (3) $\to$ City Center (4) $\to$ Station (5) with distances $4.0 + 2.8 + 3.2 =$ **10.0 km**.

**Result:**
- **BFS** selects the Direct Highway: **1 hop**, but **14.0 km** total.
- **Dijkstra** selects the Local City Route: **3 hops**, but **10.0 km** total.

Dijkstra saves 4.0 km of travel distance, proving that minimizing hops does not minimize road distance.

---

## 6. Complexity Analysis

### Time Complexity
- **Dijkstra with Binary Heap:**
  - Initializing arrays: $O(V)$.
  - Each vertex is extracted from the priority queue at most once as the active node: $V \times O(\log V) = O(V \log V)$.
  - In the worst case, every edge can trigger a relaxation that pushes a new pair into the heap: $E \times O(\log V) = O(E \log V)$.
  - **Total Time Complexity:** $\mathcal{O}((V + E) \log V)$.
  - *Note on Sparse Road Networks:* Since $E \approx k \cdot V$ where $k$ is small ($k \approx 4$), the complexity simplifies to $\mathcal{O}(V \log V)$.

- **Breadth-First Search (BFS):**
  - Every vertex enters and exits the FIFO queue once: $O(V)$.
  - Every edge is inspected once during neighbor exploration: $O(E)$.
  - **Total Time Complexity:** $\mathcal{O}(V + E)$.

### Space Complexity
- **Adjacency List:** $O(V + E)$ to store the vertices and edges.
- **Auxiliary Algorithm Memory:**
  - Dijkstra: $O(V)$ for `dist`, `parent`, `visited`, and priority queue.
  - BFS: $O(V)$ for `visited`, `parent`, `hops`, and FIFO queue.
- **Total Space Complexity:** $\mathcal{O}(V + E)$.

---

## 7. Compilation & Execution (Windows / MinGW)

The project requires a C++17 compatible compiler (MinGW GCC, Clang, or MSVC) and relies strictly on standard C++ libraries.

### Method 1: Direct MinGW g++ Command Line (Recommended)

Open **PowerShell** or **Command Prompt** in the project root directory and execute:

```powershell
# 1. Compile with C++17 standard, full warnings, and optimization
g++ -std=c++17 -Wall -Wextra -O2 -Iinclude src/main.cpp -o routex.exe

# 2. Run the compiled executable
.\routex.exe
```

### Method 2: Using CMake

```powershell
# 1. Create build directory
mkdir build
cd build

# 2. Generate build files
cmake ..

# 3. Build the target
cmake --build . --config Release

# 4. Run the application
.\RouteX.exe
```

---

## 8. Sample Input & Output

### Main Menu
```text
========================================
          ROUTEX - ROUTE FINDER         
========================================

1. Display Available Locations
2. Display Graph Connections
3. Find Shortest Route
4. Compare Pathfinding Algorithms
5. Run Performance Benchmark
6. Run System Test Suite
0. Exit

========================================
Enter your choice (0-6): 3
```

### Finding Shortest Route (Option 3)
```text
========================================
         FIND SHORTEST ROUTE (DIJKSTRA) 
========================================
Enter Source Location (by ID 0-14 or Name, or 'list' to view options): College Gate
Enter Destination Location (by ID 0-14 or Name, or 'list' to view options): Railway Station

Finding shortest route from 'College Gate' to 'Railway Station'...

----------------------------------------
Source: College Gate
Destination: Railway Station

Shortest Route:
College Gate -> Central Library -> City Center -> Railway Station

Total Distance: 10.0 km
Total Stops:    3 road segment(s)
Algorithm:      Dijkstra (with Min-Priority Queue)
Execution Time: 7.420 microseconds
Nodes Visited:  10
Edges Relaxed:  15
----------------------------------------
```

### Algorithm Comparison (Option 4)
```text
=======================================================
      ALGORITHM COMPARISON: DIJKSTRA VS BFS            
=======================================================
Source:      College Gate (ID: 0)
Destination: Railway Station (ID: 5)
-------------------------------------------------------

[1] DIJKSTRA'S ALGORITHM (Weighted Shortest Path)
    Route: College Gate -> Central Library -> City Center -> Railway Station
    Total Distance:   10.00 km
    Total Hops/Edges: 3 hops
    Nodes Visited:    10
    Edges Relaxed:    15
    Execution Time:   6.115 microseconds

[2] BREADTH-FIRST SEARCH (BFS - Unweighted Fewest Hops)
    Route: College Gate -> Railway Station
    Total Distance:   14.00 km (actual road distance along BFS path)
    Total Hops/Edges: 1 hops (MINIMAL edges)
    Nodes Visited:    5
    Execution Time:   1.842 microseconds

-----------------------------------------------------------------
                   METRIC COMPARISON SUMMARY                     
-----------------------------------------------------------------
Metric                Dijkstra                BFS                   
-----------------------------------------------------------------
Optimizes For         Min Total Distance      Min Edge/Hop Count    
Total Distance        10.00 km                14.00 km              
Total Hops (Edges)    3                       1                     
Theoretical Time      O((V + E) log V)        O(V + E)              
Auxiliary Space       O(V)                    O(V)                  
-----------------------------------------------------------------

[CRITICAL DSA INSIGHT]:
* Notice: Dijkstra found a physically SHORTER route (10.00 km vs 14.00 km),
  even though it required 3 hops compared to BFS's 1 hops.
* Why? BFS treats every road segment as identical (weight = 1).
  Dijkstra accounts for true edge weights (actual kilometers).
=======================================================
```

---

## 9. Performance Analysis & Benchmark Methodology

The RouteX benchmark suite evaluates Dijkstra's algorithm across graph sizes of approximately **50, 100, 250, and 500 vertices**.

### Benchmark Methodology
1. **Separation of Concerns:** Synthetic graphs are generated **strictly outside** the timed section. Graph memory allocation and console output do not pollute the measured algorithm runtime.
2. **Monotonic High-Precision Timing:** Uses `std::chrono::steady_clock` to protect against system clock adjustments.
3. **Adaptive Sample Scaling:** To eliminate the ~15ms timer tick granularity present on Windows operating systems, the benchmark dynamically scales test iterations (averaging over hundreds of repeated runs) until the timing sample reliably exceeds OS jitter thresholds.
4. **Validation Check:** Every benchmarked path is verified for continuity, start/end vertex correctness, and floating-point cumulative distance integrity.

### Measured Empirical Benchmark Results

| Vertices ($V$) | Edges ($E$) | Graph Density (%) | Avg Execution Time ($\mu\text{s}$) | Theoretical Complexity | Result Valid? |
| :---: | :---: | :---: | :---: | :---: | :---: |
| **50** | 100 | 8.16% | **7.79 $\mu\text{s}$** | $O((V + E) \log V)$ | **YES (Passed)** |
| **100** | 200 | 4.04% | **13.08 $\mu\text{s}$** | $O((V + E) \log V)$ | **YES (Passed)** |
| **250** | 500 | 1.60% | **67.37 $\mu\text{s}$** | $O((V + E) \log V)$ | **YES (Passed)** |
| **500** | 1000 | 0.80% | **147.06 $\mu\text{s}$** | $O((V + E) \log V)$ | **YES (Passed)** |

*Observation:* As the number of vertices increases tenfold (from 50 to 500), execution time scales gracefully from $\approx 7.8\ \mu\text{s}$ to $\approx 147\ \mu\text{s}$, confirming the near-linear behavior expected of Dijkstra on sparse networks.

---

## 10. Viva Examination Q&A (15 Questions)

### Q1: What is the main objective of this project?
> **Answer:** The primary objective is to implement and demonstrate Dijkstra's algorithm using a Min-Priority Queue to solve the single-pair shortest path problem on a road network, contrast its behavior with Breadth-First Search (BFS), and empirically analyze its scaling characteristics across graphs of varying sizes.

### Q2: Why did you choose an Adjacency List over an Adjacency Matrix?
> **Answer:** Road networks are sparse graphs where each vertex connects only to an average of 3 to 5 neighbors. An adjacency matrix consumes $O(V^2)$ memory and takes $O(V)$ time to find neighbors. An adjacency list uses only $O(V + E)$ memory and allows exploring neighbors in $O(\text{deg}(u))$ time, which is substantially faster.

### Q3: How does Dijkstra's algorithm decide which vertex to visit next?
> **Answer:** It uses a greedy approach. It maintains tentative distances for all discovered vertices in a Min-Priority Queue (Min-Heap). In each iteration, it extracts the vertex $u$ with the minimum tentative distance that has not yet been finalized.

### Q4: Why can't Dijkstra's algorithm handle negative edge weights?
> **Answer:** Dijkstra is a greedy algorithm based on the premise that once a vertex is marked finalized (popped from the priority queue), its shortest path distance is permanently optimal. If a negative weight edge exists later in the graph, it could decrease the distance of an already finalized vertex, violating this greedy assumption. For graphs with negative weights, the Bellman-Ford algorithm must be used.

### Q5: Can Breadth-First Search (BFS) be used to find the shortest path in a weighted graph?
> **Answer:** No. Standard BFS assumes all edges have equal weight (weight = 1). BFS finds the path with the minimum *number of edges* (hops), not the path with the minimum *sum of weights*. In RouteX, we demonstrated that a 1-hop path of 14 km is chosen by BFS over a 3-hop path of 10 km, which Dijkstra correctly identifies.

### Q6: What is "edge relaxation" in Dijkstra's algorithm?
> **Answer:** Relaxation is the process of checking whether moving to vertex $v$ through vertex $u$ improves the currently known shortest distance to $v$. If $\text{dist}[u] + \text{weight}(u, v) < \text{dist}[v]$, we update $\text{dist}[v] = \text{dist}[u] + \text{weight}(u, v)$ and set $\text{parent}[v] = u$.

### Q7: Why do we push duplicate vertices into the C++ `std::priority_queue`?
> **Answer:** The standard C++ STL `std::priority_queue` does not support a `decrease_key` operation. When a shorter path to an already queued vertex is found, we push a new `(new_dist, v)` pair into the heap. To prevent reprocessing, we check `if (currDist > dist[u]) continue;` or maintain a `visited` array to discard stale entries when popped.

### Q8: What is the time complexity of Dijkstra's algorithm with a binary min-heap?
> **Answer:** $\mathcal{O}((V + E) \log V)$. There are $V$ extract-min operations ($V \log V$) and at most $E$ decrease-key/push operations ($E \log V$). For sparse road graphs where $E \approx k \cdot V$, this simplifies to $\mathcal{O}(V \log V)$.

### Q9: How does RouteX reconstruct the actual path of locations from source to destination?
> **Answer:** It maintains a `parent` array where `parent[v]` stores the predecessor vertex that relaxed $v$. Once destination $D$ is reached, the algorithm backtracks from $D$ to the source through parent pointers (`curr = parent[curr]`) and reverses the resulting list.

### Q10: How are unreachable destinations handled in RouteX?
> **Answer:** The `dist` vector is initialized to infinity (`std::numeric_limits<double>::infinity()`). If the priority queue empties without reaching the destination (or if `dist[dest] == INF`), the destination belongs to a disconnected component. The program checks this and gracefully informs the user without throwing exceptions or crashing.

### Q11: Why is `std::chrono::steady_clock` preferred over `std::chrono::system_clock` for benchmarking?
> **Answer:** `system_clock` represents wall-clock time and can be modified by the operating system, NTP synchronization, or daylight saving adjustments, causing negative or skewed durations. `steady_clock` is guaranteed to be monotonic and steadily advancing, making it the correct choice for benchmarking code execution.

### Q12: How did you benchmark Dijkstra without I/O interference?
> **Answer:** Graph creation and console printing (`std::cout`) are placed outside the timed section. Only the pure algorithm invocation (`runDijkstraCore`) is measured within the clock start and end timestamps.

### Q13: What happens if source and destination are the exact same location?
> **Answer:** RouteX handles this as an $O(1)$ boundary check: it immediately returns `found = true`, `totalDistance = 0.0`, and path `[src]`, without running unnecessary heap operations.

### Q14: How does RouteX validate that a benchmarked shortest path is correct?
> **Answer:** The `validatePath()` function checks that:
> 1. The path starts at the source and ends at the destination.
> 2. Every consecutive pair of vertices $(u, v)$ in the path has a valid edge in the adjacency list.
> 3. The sum of the individual road lengths equals the reported `totalDistance`.

### Q15: What is the A* (A-Star) search algorithm, and how does it relate to Dijkstra?
> **Answer:** A* is an extension of Dijkstra’s algorithm that incorporates a heuristic function $h(u)$ estimating the remaining distance from $u$ to the destination (such as straight-line Euclidean distance). While Dijkstra expands in all directions equally, A* biases the exploration frontier toward the destination, reducing the number of nodes evaluated. Dijkstra is effectively A* with $h(u) = 0$.
