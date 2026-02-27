# Skyline Computation with BJR-Tree

**Course:** Data Structures  
**Instructor:** Dr. Ahmadali Aeen  
**Semester:** Spring 2025  
**University:** Shahid Beheshti University  

---

## 📌 Project Overview

This project implements a **state-of-the-art (SOTA)** solution for **Skyline computation** in multi-dimensional datasets using the **BJR-Tree** data structure. The implementation is based on the research paper and includes all core algorithms for efficient skyline query processing.

The skyline operator filters out data points that are dominated by others, returning only the **non-dominated points** (optimal items) based on multiple criteria. This is particularly useful in **recommendation systems**, **multi-criteria decision making**, and **database query optimization**.

---

## 🧠 What is Skyline?

Given a set of multi-dimensional points, a point **dominates** another if it is at least as good in all dimensions and strictly better in at least one dimension. The **skyline** consists of points that are not dominated by any other point.

### Example: Hotel Recommendation System
- **Dimensions:** Price (lower is better), Distance to sea (lower is better)
- **Skyline:** Hotels that are not worse than any other hotel in both price and distance

### Domination Definition
For a point `p` to dominate point `q`:
- `p` is better than or equal to `q` in all dimensions
- `p` is strictly better than `q` in at least one dimension

---

## 🚀 Features Implemented

### 1. **BJR-Tree Data Structure**
- Custom tree structure for efficient skyline maintenance
- Optimized for dynamic datasets (insertion and deletion)
- Supports multi-dimensional points (up to 7 dimensions)
- Maintains parent-child relationships and subtree sizes

### 2. **Core Algorithms**

| Algorithm | Description | Implementation |
| :--- | :--- | :--- |
| **Injection** | Inserts new points into the BJR-Tree while maintaining skyline properties | `inject()` function |
| **Ejection** | Removes points from the tree and updates the skyline | `eject()` function |
| **Lazy Evaluation** | Optimizes tree balancing during injection with depth threshold | `injectLazy()` function |
| **Domination Check** | Determines if one point dominates another | `dominate()` method |

### 3. **Dynamic Dataset Support**
- Handles **small, medium, and large** dataset sizes
- Supports up to **800,000 entries** with up to **7 dimensions**
- Dynamic time steps for evolving datasets
- Points have active intervals (start time, end time)

---

## 📁 Dataset Characteristics

The implementation is tested on datasets with the following specifications:

| Dataset | Entries | Dimensions | Time Steps | Max Skyline | Input Size | Output Size |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **small** | 1,000 | 4 | 10 | 934 | 48 KB | 15 KB |
| **medium** | 50,000 | 5 | 10 | 998 | 310500 KB | 14 MB |
| **large** | 800,000 | 7 | 40 | 2008 | 811 MB | 329 MB |

### Dataset Files Format
Each dataset comes with three files:
- `[size].setup` - Configuration (number of points, dimensions, time steps)
- `[size].input` - Point coordinates (one point per line)
- `[size].times` - Active intervals for each point (start end)
- `[size].refout` - Reference output for validation

---

## 🛠️ Technologies Used

- **Language:** C++ (C++11 standard)
- **Data Structures:** BJR-Tree, Custom tree implementation with parent pointers
- **Algorithms:** Injection, Ejection, Lazy Evaluation, Domination Check
- **File Processing:** Large dataset handling (up to 800MB)
- **Memory Management:** Dynamic allocation with proper cleanup

---

## 📋 Prerequisites

- C++ compiler with C++11 support (g++ recommended)
- Make (optional, for build automation)
- Sufficient memory for large dataset processing (at least 4GB RAM for large dataset)

---

