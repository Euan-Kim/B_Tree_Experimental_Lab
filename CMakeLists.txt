cmake_minimum_required(VERSION 3.10)

project(BTreeExperimentalLab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(SOURCES
    src/student.cpp
    src/io.cpp
    src/node.cpp
    src/btree.cpp
    src/bstartree.cpp
    src/bplustree.cpp
    src/bulkloading.cpp
    src/experiments.cpp
    src/main.cpp
)

add_executable(btel ${SOURCES})

if(MSVC)
    target_compile_options(btel PRIVATE /w)
else()
    target_compile_options(btel PRIVATE -w)
endif()