cmake_minimum_required (VERSION 2.8.11)

include_directories(include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g3")

add_library(cyberbird STATIC cyberbird.cc storage.cc data.cc indexer.cc tree.cc index_page.cc writer.cc reader.cc)

find_package(ZLIB REQUIRED)
if (ZLIB_FOUND)
    target_link_libraries(cyberbird ${ZLIB_LIBRARIES})
endif(ZLIB_FOUND)
