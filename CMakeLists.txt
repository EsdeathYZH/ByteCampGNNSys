cmake_minimum_required(VERSION 3.10)
project(ByteGraph)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=1 -fno-omit-frame-pointer)
add_definitions(-std=c++11)
option(BUILD_TEST "build test" ON)
option(DEBUG_MODE "debug mode" ON)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH}) # To allow CMake to locate our Find*.cmake files

if (DEBUG_MODE)
    add_compile_options(-g)
    message(STATUS "debug mode")
else ()
    add_compile_options(-O2)
    message(STATUS "release mode")
endif ()

find_package(Threads REQUIRED)

find_package(Thrift REQUIRED)
include_directories(${THRIFT_INCLUDE_DIR})

find_package(LibEvent REQUIRED)
include_directories(${LIBEVENT_INCLUDE_DIR})

find_package(OpenMP)
if (OPENMP_FOUND)
    message("find openmp")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

message("use Thrift NonblockingServer")
set(NONBLOCKING_LIBS
        ${THRIFT_NB_LIBRARIES}
        ${LIBEVENT_LIBRARIES}
        )


include_directories("${PROJECT_SOURCE_DIR}/src")
# include_directories("${PROJECT_SOURCE_DIR}/gen-cpp")

add_subdirectory(src)
add_subdirectory(third_party/glog)
if (BUILD_TEST)
    enable_testing()

    set(install_gtest OFF)
    set(install_gmock OFF)
    set(build_gmock ON)

    set(GTEST_ROOT_DIR ${PROJECT_SOURCE_DIR}/third_party/googletest)
    add_subdirectory("${PROJECT_SOURCE_DIR}/third_party/googletest")
    set(GTEST_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/third_party/googletest/googletest/include"
            "${PROJECT_SOURCE_DIR}/third_party/googletest/googlemock/include")

    add_subdirectory(test)
endif(BUILD_TEST)