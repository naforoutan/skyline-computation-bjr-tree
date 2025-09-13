# Dynamic Skyline Computation with BJR-Tree

A Data Structures project implementing BJR-Tree for dynamic skyline computation, processing multi-dimensional datasets with continuous point insertions and deletions to maintain optimal skyline results over time. The main program is compiled from ```main.cpp``` while the core BJR-Tree structure is implemented in ```modules.hpp```.

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
# Should configure then run main.cpp
```

## Configuration Options
Setting	| Values |	Description
|----|----|----|
version	|"small", "medium", "large"	| Choose dataset size
use_lazy| 	true, false	| Enable/disable lazy evaluation
use_nd_cache |	true, false	| Enable/disable ND-cache optimization



🎓 Academic Project
Developed for Data Structures course (Spring 2025). Implements BJR-Tree algorithms for dynamic skyline computation.
