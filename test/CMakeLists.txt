
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

bytegraph_test("sample_test.cc")

set(THRIFT_SRC
        ../src/gen-cpp/ByteGraph_types.h
        ../src/gen-cpp/ByteGraph_types.cpp
        ../src/gen-cpp/GraphServices.h
        ../src/gen-cpp/GraphServices.cpp
        )
execute_process(COMMAND ${THRIFT_COMPILER} --gen cpp -r ${CMAKE_CURRENT_SOURCE_DIR}/thrift-idls/ByteGraph.thrift
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE output
        ERROR_VARIABLE error)

set(CLIENT_SRC
        ../src/client/client_base.h
        ../src/client/cache_base.h
        ../src/client/rpc_client.h
        ../src/client/rpc_client.cpp
        ../src/client/client_without_cache.h
        ../src/client/client_without_cache.cpp
        ../src/client/client_with_cache.h
        ../src/client/client_with_cache.cpp
        ../src/client/simple_cache.h
        ../src/client/simple_cache.cpp
        ../src/utils/random.h
        ../src/utils/utils.h
        ${THRIFT_SRC}
        )

add_library(MethodTest STATIC ${CLIENT_SRC})
target_link_libraries(MethodTest
        PUBLIC glog::glog
        PRIVATE ${THRIFT_LIBRARIES} ${NONBLOCKING_LIBS}
        )
add_executable(Method MethodTest.cpp MethodTest.h)
target_link_libraries(Method PRIVATE MethodTest)
