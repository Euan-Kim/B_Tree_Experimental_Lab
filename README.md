# B Tree Experimental Lab v1.0 (Assignment 1 of CSE321)

A C++ experimental laboratory designed to implement and compare various B-Tree data structures, including B-Tree, B*-Tree, and B+-Tree.

## 1. Project Structure

The project is organized into `include` for header files and `src` for implementation files. The directory layout is outlined below.

### Directory Layout

```text
.
├── CMakeLists.txt        # Build configuration
├── student.csv           # Csv file for input
├── include/              # Header files
│   ├── student.h         # Basic data structure for Student information
│   ├── io.h              # ReadStudentCsv implementation
│   ├── node.h            # Tree node definitions
│   ├── btree.h           # Base B-Tree implementation
│   ├── bstartree.h       # B*-Tree implementation
│   ├── bplustree.h       # B+-Tree implementation
│   └── experiments.h     # Benchmarking logic
└── src/                  # Implementation files
    ├── student.cpp       # Storing student information logic
    ├── io.cpp            # Function for reading csv file
    ├── node.cpp          # Node logic
    ├── btree.cpp         # B-Tree logic
    ├── bstartree.cpp     # B*-Tree logic
    ├── bplustree.cpp     # B+-Tree logic
    ├── bulkloading.cpp   # Bulk loading algorithms
    ├── experiments.cpp   # Performance analysis logic
    └── main.cpp          # Entry point
```

## 2. Prerequisites

To build and run this project on a Linux environment, you need the following:

Compiler: A C++ compiler supporting the C++17 standard (GCC recommanded).

Build System: CMake version 3.10 or higher.

Build Tools: make required.

## 3. Build Instructions

Use CMake to generate the build files and compile the executable. Run the following commands in your terminal:

```Bash
# 1. Generate build system (output to 'build' directory)
cmake -S . -B build

# 2. Compile the project
cmake --build build
```

## 4. How to Run

Once the build is successful, the executable btel will be located in the build directory.

```Bash
# Execute the program
./build/btel
```

## 5. Usage

