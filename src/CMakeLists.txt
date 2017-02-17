cmake_minimum_required (VERSION 2.8.11)

include_directories(include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g3")

add_library(cyberbird STATIC cyberbird.cc storage.cc data.cc indexer.cc)
