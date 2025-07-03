# Advanced Data Structure Assignments

This repository contains my assignments and experimental reports for the NTU Advanced Data Structures (AdvDS) course, Spring 2025. Each report includes theoretical insights, algorithm implementations, and empirical analyses on different topics in data structures.

## Contents

### Linear-Time DSU and Parallel Union-Find
- **Paper**: _A Linear-Time Algorithm for a Special Case of Disjoint Set Union_ by Gabow & Tarjan (1985)
- **Focus**: 
  - Summary of Gabow-Tarjan’s linear-time DSU
  - Comparison with modern parallel & distributed union-find algorithms
  - Discussion on performance trade-offs in modern computing environments

---

### BST Upper and Lower Bounds: Empirical Comparison
- **Goal**: Compare three BST variants (Basic, Splay, Tango) against Wilber I lower bound using multiple access patterns.
- **Access Sequences**: Random, Monotonic, Hotspot, Zigzag, Bit-reversal
- **Evaluation Metrics**:
  - Comparisons vs. Wilber bound
  - Rotations
  - Execution Time (ms)
- **Key Findings**:
  - Tango Tree consistently tracks Wilber I closely.
  - Splay Tree adapts well in hotspot/structured cases.
  - Basic BST serves as a control group.


---

### Streaming Algorithms: Morris++, FM++, HyperLogLog
- **Implemented Algorithms**:  
  - Morris++ (stream length estimation)  
  - FM++ (distinct count via hash sketches)  
  - HyperLogLog (practical and memory-efficient cardinality estimation)
- **Experiments**:  
  - Stream types: all-distinct, all-same, random duplicates  
  - Metrics: Mean Relative Error, 99th Percentile Error, Memory Usage
- **Findings**:
  - HLL has best overall accuracy and space efficiency
  - FM++ is fragile under low entropy
  - Morris++ is robust but not for distinct counting
- **Tools**: Python for sketch simulation and plotting

---

## 🛠 Environment

- C++17 for algorithm implementation
- Python 3.10+ for experiment visualization
- Matplotlib / NumPy for plotting
