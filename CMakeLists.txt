cmake_minimum_required (VERSION 2.8.11)

project(cyberbird)

add_subdirectory(src)
add_subdirectory(test)

enable_testing()
add_test(NAME cyberbirdTest COMMAND Test)

