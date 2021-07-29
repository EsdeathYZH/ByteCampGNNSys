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
        utils/random.h
        ${THRIFT_SRC}
        )

set(SERVER_SRC
        server/worker.h
        server/worker.cpp
        graph/graph_loader.h
        graph/graph.h
        utils/random.h
        engine/sampling_engine.h
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
        ${THRIFT_LIBRARIES}
        ${NONBLOCKING_LIBS}
        )
