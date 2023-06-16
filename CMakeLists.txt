cmake_minimum_required(VERSION 3.0)
project(YourProjectName)

# Set the C++ standard (optional)
set(CMAKE_CXX_STANDARD 11)

# Add source files to be compiled
set(SOURCES
    btree/src/main.cpp
)

set(SOURCES_BENCH
    btree/src/bench_btree.cpp
)

set(SOURCES_SUCCINCT
    btree/src/succinct_btree.cpp
)

# Add header files to include directories
include_directories(include)

# Create an executable from the sources
add_executable(gapped ${SOURCES})
add_executable(packed ${SOURCES_BENCH})
add_executable(succinct ${SOURCES_SUCCINCT})

find_package(Snappy)
find_path(SNAPPY_INCLUDE_DIR NAMES snappy.h PATHS compressions/snappy)
find_library(SNAPPY_LIBRARY NAMES snappy PATHS compressions/snappy/build)
include_directories(${SNAPPY_INCLUDE_DIR})
target_link_libraries(succinct ${SNAPPY_LIBRARY})

find_path(LZ4_INCLUDE_DIR NAMES lz4.h PATHS compressions/lz4/lib)
find_library(LZ4_LIBRARY NAMES lz4 PATHS compressions/lz4/lib)
include_directories(${LZ4_INCLUDE_DIR})
target_link_libraries(succinct ${LZ4_LIBRARY})

find_path(FASTPFOR_INCLUDE_DIR NAMES fastpfor.h codecfactory.h deltautil.h PATHS compressions/FastPFor/headers)
find_library(FASTPFOR_LIBRARY NAMES FastPFOR PATHS compressions/FastPFor/build)
include_directories(${FASTPFOR_INCLUDE_DIR})
target_link_libraries(succinct ${FASTPFOR_LIBRARY})
