# RouteX: Test Cases & Validation Suite

This document outlines the test cases designed to verify the correctness, edge-case resilience, and performance characteristics of **RouteX**.

---

## Summary of Test Results

| Test Case ID | Test Category | Description | Expected Output | Actual Output | Status |
| :--- | :--- | :--- | :--- | :--- | :---: |
| **TC-01** | Known Shortest Path | Finding shortest route between College Gate (0) and Railway Station (5) | Route: `0 -> 3 -> 4 -> 5`<br>Distance: `10.0 km` | Route: `0 -> 3 -> 4 -> 5`<br>Distance: `10.0 km` | **PASSED** |
| **TC-02** | Multiple Routes / Trade-off | Comparison of Dijkstra (min distance) vs BFS (min hops) on same pair (0 -> 5) | Dijkstra: `10.0 km` (3 hops)<br>BFS: `14.0 km` (1 hop) | Dijkstra: `10.0 km` (3 hops)<br>BFS: `14.0 km` (1 hop) | **PASSED** |
| **TC-03** | Unreachable Destination | Querying route to isolated node (Remote Satellite Campus, ID 14) | "Destination is UNREACHABLE"<br>`found = false` | "Destination is UNREACHABLE"<br>`found = false` | **PASSED** |
| **TC-04** | Invalid Node Input | Passing out-of-bounds IDs (`-1`, `99`) or unknown names (`Atlantis`) | Error notification & safe reprompt, zero crash | Graceful error prompt, no crash | **PASSED** |
| **TC-05** | Single Vertex / Zero Distance | Source equals Destination (e.g., Central Library 3 -> 3) | Distance: `0.0 km`<br>Path: `[3]` | Distance: `0.0 km`<br>Path: `[3]` | **PASSED** |
| **TC-06** | Zero-Weight Edges | Path containing zero-weight tram connection | Correctly chooses zero-weight shortcut | Optimal distance computed without cycles | **PASSED** |
| **TC-07** | Performance Benchmark | Stress test on synthetic graphs of $V \in \{50, 100, 250, 500\}$ | Valid paths; runtime scaling near $O(V \log V)$ | All paths validated; sub-millisecond execution | **PASSED** |

---

## Detailed Test Case Walkthroughs

### TC-01: Known Shortest Path with Multiple Interconnected Paths
* **Objective:** Verify that Dijkstra's algorithm selects the route with the minimum cumulative edge weight among several alternate paths.
* **Input Graph Scenario:**
  - Route 1: College Gate (0) -> Innovation Hub (1) -> Tech Park (2) -> Railway Station (5)
    - Distance: $2.5 + 3.5 + 5.0 = 11.0\text{ km}$
  - Route 2: College Gate (0) -> Innovation Hub (1) -> City Center (4) -> Railway Station (5)
    - Distance: $2.5 + 6.0 + 3.2 = 11.7\text{ km}$
  - Route 3: College Gate (0) -> Central Library (3) -> City Center (4) -> Railway Station (5)
    - Distance: $4.0 + 2.8 + 3.2 = 10.0\text{ km}$ (Optimal)
  - Route 4: College Gate (0) -> Railway Station (5) via Express Highway
    - Distance: $14.0\text{ km}$
* **Input:** Source = `College Gate` (0), Destination = `Railway Station` (5)
* **Expected Result:**
  ```text
  Shortest Route: College Gate -> Central Library -> City Center -> Railway Station
  Total Distance: 10.0 km
  ```
* **Status:** PASSED.

---

### TC-02: BFS vs. Dijkstra Objective Comparison
* **Objective:** Demonstrate that BFS optimizes for minimum edge count (hop count), while Dijkstra optimizes for minimum edge weight.
* **Input:** Source = `0`, Destination = `5`
* **Algorithm Trace:**
  - **BFS:** Explores layer-by-layer. Since an edge exists directly connecting vertex 0 to vertex 5, BFS discovers it in 1 hop and terminates. Road distance = $14.0\text{ km}$.
  - **Dijkstra:** Min-priority queue pulls nodes by least tentative distance. It discovers the 3-hop path through nodes 3 and 4 with total weight $10.0\text{ km}$.
* **Expected Result:**
  - Dijkstra Distance: $10.0\text{ km}$ (3 hops)
  - BFS Distance: $14.0\text{ km}$ (1 hop)
* **Status:** PASSED. Confirms BFS cannot be used for weighted shortest paths when weights differ.

---

### TC-03: Unreachable Destination Handling
* **Objective:** Ensure the system detects disconnected graph components without hanging in an infinite loop or causing segmentation faults.
* **Input:** Source = `College Gate` (0), Destination = `Remote Satellite Campus` (14)
* **Expected Result:**
  ```text
  Result: Destination is UNREACHABLE.
  No route exists between these two locations in the network.
  ```
* **Status:** PASSED. Returns `found = false` and prints educational notice.

---

### TC-04: Robust Input Validation
* **Objective:** Verify terminal stability against unexpected user input types.
* **Test Sub-cases:**
  1. Non-numeric input at menu: `abc` -> Clears `std::cin` fail-state, discards buffer, prompts again.
  2. Out-of-bounds menu selection: `99` -> Displays range validation error, prompts again.
  3. Non-existent location name: `Atlantis` -> Informs user location not found, advises typing `list`.
* **Status:** PASSED. Zero terminal crashes or hanging states.

---

### TC-05: Single Vertex (Source equals Destination)
* **Objective:** Verify boundary condition when user starts and finishes at the same vertex.
* **Input:** Source = `Central Library` (3), Destination = `Central Library` (3)
* **Expected Result:**
  ```text
  Total Distance: 0.0 km
  Total Stops: 0 road segment(s)
  Route: Central Library
  ```
* **Status:** PASSED.

---

### TC-06: Zero-Weight Edge Handling
* **Objective:** Verify Dijkstra's algorithm correctness in graphs containing edges with weight $0.0$ (e.g., zero-cost transit links / free shuttles).
* **Graph:**
  - Vertex A (0) -> Vertex B (1): weight = 0.0
  - Vertex B (1) -> Vertex C (2): weight = 5.0
  - Vertex A (0) -> Vertex C (2): weight = 8.0
* **Expected Result:** Path `A -> B -> C` chosen with total distance $5.0$.
* **Status:** PASSED. Dijkstra correctly handles non-negative edges ($\ge 0$).

---

### TC-07: Scalability Stress Benchmark
* **Objective:** Measure empirical scaling across graph orders $V \in \{50, 100, 250, 500\}$.
* **Verification:**
  - Each synthetic graph is generated outside the timing block.
  - Path correctness is verified by summing edges along the reconstructed path and comparing against the reported minimum distance.
* **Measured Benchmark Results:**
  - $V = 50, E = 100 \implies \approx 7.8\ \mu\text{s}$
  - $V = 100, E = 200 \implies \approx 13.1\ \mu\text{s}$
  - $V = 250, E = 500 \implies \approx 67.4\ \mu\text{s}$
  - $V = 500, E = 1000 \implies \approx 147.1\ \mu\text{s}$
* **Status:** PASSED. All paths mathematically validated.
