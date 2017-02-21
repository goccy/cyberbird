cmake_minimum_required (VERSION 2.8.11)

include_directories(../src/include)

# set runtime directory
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g3")

add_subdirectory(lib/googletest)

# declare GTEST_ROOT
set(GTEST_ROOT lib/googletest/googletest/)

# Include files directory
include_directories(${GTEST_ROOT}/include/)

# Library directory
link_directories(${GTEST_ROOT}/build/)

# Generate executable file
add_executable(Test test_indexer.cc test_tree.cc test_index_page.cc test_storage.cc)

# Link libraries
target_link_libraries(Test cyberbird gtest gtest_main pthread)
