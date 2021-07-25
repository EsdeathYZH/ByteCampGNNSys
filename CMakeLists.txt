cmake_minimum_required(VERSION 3.10)
project(ByteGraph)

add_definitions(-D_GLIBCXX_USE_CXX11_ABI=1 -fno-omit-frame-pointer)
option(BUILD_TEST "build test" ON)
option(DEBUG_MODE "debug mode" ON)

if (DEBUG_MODE)
    add_compile_options(-g)
    message(STATUS "debug mode")
else ()
    add_compile_options(-O2)
    message(STATUS "release mode")
endif ()

include_directories("${PROJECT_SOURCE_DIR}/src")

add_executable(GraphClient
    "${PROJECT_SOURCE_DIR}/src/client/client.cpp"
)
target_link_libraries(GraphClient
        PRIVATE glog::glog)

add_executable(GraphServer
    "${PROJECT_SOURCE_DIR}/src/server/worker.cpp"
)

if (BUILD_TEST)
    enable_testing()

    set(install_gtest OFF)
    set(install_gmock OFF)
    set(build_gmock ON)

    add_subdirectory("third_party/googletest")

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

