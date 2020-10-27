cmake_minimum_required(VERSION 3.12)

project(e2e-lib)
set(CMAKE_CXX_STANDARD 14)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

add_subdirectory(src)
enable_testing()
add_subdirectory(tests)
