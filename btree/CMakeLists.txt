cmake_minimum_required(VERSION 3.0)
project(YourProjectName)

# Set the C++ standard (optional)
set(CMAKE_CXX_STANDARD 11)

# Add source files to be compiled
set(SOURCES
    src/main.cpp
)

set(SOURCES_BENCH
    src/bench_btree.cpp
)


# Add header files to include directories
include_directories(include)

# Create an executable from the sources
add_executable(${PROJECT_NAME} ${SOURCES})
add_executable(Executable2 ${SOURCES_BENCH})

find_package(Snappy)

find_path(SNAPPY_INCLUDE_DIR NAMES snappy.h)
find_library(SNAPPY_LIBRARY NAMES snappy)

include_directories(${SNAPPY_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${SNAPPY_LIBRARY})


find_path(LZ4_INCLUDE_DIR NAMES lz4.h)
find_library(LZ4_LIBRARY NAMES lz4)

include_directories(${LZ4_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${LZ4_LIBRARY})

find_path(FASTPFOR_INCLUDE_DIR NAMES fastpfor.h)
find_path(FASTPFOR_INCLUDE_CODECFACTORY NAMES codecfactory.h)
find_path(FASTPFOR_INCLUDE_DELTAUTIL NAMES deltautil.h)
find_library(FASTPFOR_LIBRARY NAMES FastPFOR)

include_directories(${FASTPFOR_INCLUDE_DIR})
include_directories(${FASTPFOR_INCLUDE_CODECFACTORY})
include_directories(${FASTPFOR_INCLUDE_DELTAUTIL})
target_link_libraries(${PROJECT_NAME} ${FASTPFOR_LIBRARY})