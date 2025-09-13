# Dynamic Skyline Computation with BJR-Tree

A high-performance C++ implementation of dynamic skyline computation using the BJR-Tree data structure. Efficiently processes multi-dimensional datasets with continuous insertions and deletions over time.

## 📥 Dataset Download
The dataset is not included in this repository. Download it from:

🔗 https://s33.picofile.com/file/8484976226/dataset.rar.html

After downloading, extract the files into the project structure:

```
dataset/
├── small/
├── medium/
└── large/
```

##  Key Features
- Dynamic Processing: Handles real-time point injections and ejections
- BJR-Tree Implementation: Optimized tree structure for skyline maintenance
- Multiple Optimizations:

    - Lazy evaluation for tree balancing
    - ND-cache for fast dominance checks
    - Time-window caching

- Scalable: Supports up to 800,000 points across 40,000+ timesteps
- Multi-dimensional: Works with 4D-7D datasets

## Quick Start

```bash
# Clone & setup
git clone https://github.com/your-username/skyline-computation-bjr-tree.git
cd skyline-computation-bjr-tree
mkdir -p dataset/small dataset/medium dataset/large

# Add downloaded dataset files to appropriate folders
# Then build and run
make
./skyline_computation --dataset medium --lazy true
```

## Supported Datasets
Dataset |	Points |	Dimensions	| Timesteps
--- | --- | --- | ---
Small |	1,000	| 4	| 100
Medium |50,000 | 5 | 	10,998
Large	| 800,000 |	7 |	40,998

🎓 Academic Project
Developed for Data Structures course (Spring 2025). Implements BJR-Tree algorithms for dynamic skyline computation.
