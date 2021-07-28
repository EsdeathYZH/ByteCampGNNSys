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
        client/client.h
        client/client.cpp
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
