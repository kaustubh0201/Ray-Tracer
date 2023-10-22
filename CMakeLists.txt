cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(pdc VERSION 0.0)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
find_package(OpenMP REQUIRED)

conan_basic_setup(TARGETS)

set(LIB_NAME ${CMAKE_PROJECT_NAME}lib)
set(DBG_LIB_NAME ${LIB_NAME}_dbg)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/craytracer)

include_directories(libs/include)
add_subdirectory(craytracer)

enable_testing()
add_subdirectory(tests)
