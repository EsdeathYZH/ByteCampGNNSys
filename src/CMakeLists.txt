set(THRIFT_SRC
        gen-cpp/ByteGraph_types.h
        gen-cpp/ByteGraph_types.cpp
        gen-cpp/GraphServices.h
        gen-cpp/GraphServices.cpp
        )
execute_process(COMMAND ${THRIFT_COMPILER} --gen cpp -r ${CMAKE_CURRENT_SOURCE_DIR}/thrift-idls/ByteGraph.thrift
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE output
        ERROR_VARIABLE error)

set(CLIENT_SRC
        client/client_base.h
        client/cache_base.h
        client/client_without_cache.h
        client/client_without_cache.cpp
        client/client_with_cache.h
        client/client_with_cache.cpp
        client/simple_cache.h
        client/simple_cache.cpp
        utils/random.h
        utils/config.h
        ${THRIFT_SRC}
        )

set(SERVER_SRC
        server/worker.h
        server/worker.cpp
        graph/graph.h
        graph/graph.cpp
        utils/random.h
        engine/graph_engine.h
        engine/graph_engine.cpp
        engine/its_sampling.h
        ${THRIFT_SRC}
        )


add_library(Client STATIC ${CLIENT_SRC})
target_link_libraries(Client
        PUBLIC glog::glog
        PRIVATE ${THRIFT_LIBRARIES} ${NONBLOCKING_LIBS}
        )
add_executable(GraphClient client/graph_client.cc)
target_link_libraries(GraphClient PRIVATE Client)

add_executable(GraphServer
        # "${PROJECT_SOURCE_DIR}/src/server/worker.cpp"
        ${SERVER_SRC}
        )

target_link_libraries(GraphServer
        PUBLIC glog::glog
        PRIVATE ${THRIFT_LIBRARIES} ${NONBLOCKING_LIBS}
        )
