cmake_minimum_required (VERSION 2.8.11)

include_directories(src/include)

set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -O0 -g3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g3")

add_executable(cyberbird main.cc src/cyberbird.cc src/storage.cc src/data.cc)
