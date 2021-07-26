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

message("use Thrift NonblockingServer")
set(NONBLOCKING_LIBS
    ${THRIFT_NB_LIBRARIES}
    ${LIBEVENT_LIBRARIES}
)

set(CLIENT_SRC
    src/client/client.h
    src/client/client.cpp
    src/utils/random.h
    gen-cpp/ByteGraph_types.h
    gen-cpp/GraphServices.h
    gen-cpp/GraphServices.cpp
)

set(SERVER_SRC
    src/server/worker.h
    src/server/worker.cpp
    src/graph/graph_loader.h
    src/graph/graph.h
    src/utils/random.h
    src/engine/sampling_engine.h
    gen-cpp/ByteGraph_types.h
    gen-cpp/GraphServices.h
    gen-cpp/GraphServices.cpp
)

include_directories("${PROJECT_SOURCE_DIR}/src")
# include_directories("${PROJECT_SOURCE_DIR}/gen-cpp")

add_executable(GraphClient
    # "${PROJECT_SOURCE_DIR}/src/client/client.cpp"
    ${CLIENT_SRC}
)
target_link_libraries(GraphClient
    PRIVATE glog::glog
    ${THRIFT_LIBRARIES}
    ${NONBLOCKING_LIBS}
)

add_executable(GraphServer
    # "${PROJECT_SOURCE_DIR}/src/server/worker.cpp"
    ${SERVER_SRC}
)

target_link_libraries(GraphServer
    ${THRIFT_LIBRARIES}
    ${NONBLOCKING_LIBS}
)

if (BUILD_TEST)
    enable_testing()

    set(install_gtest OFF)
    set(install_gmock OFF)
    set(build_gmock ON)

    set(GTEST_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest)
    add_subdirectory("third_party/googletest")
    set(GTEST_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest/googletest/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest/googlemock/include")

    function(bytegraph_test test_file)
        get_filename_component(test_target_name "${test_file}" NAME_WE)

        add_executable("${test_target_name}" "")
        target_sources("${test_target_name}"
                PRIVATE
                "${test_file}"
                )
        target_link_libraries("${test_target_name}" gmock gtest)
        target_compile_options("${test_target_name}" PUBLIC -g)
        add_test(NAME "${test_target_name}" COMMAND "${test_target_name}")
    endfunction(bytegraph_test)

    bytegraph_test("src/client/sample_test.cc")
endif(BUILD_TEST)

add_subdirectory(third_party/glog)

