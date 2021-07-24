cmake_minimum_required(VERSION 3.10)
project(ByteGraph)

include_directories("${PROJECT_SOURCE_DIR}/src")

add_executable(GraphClient
    "${PROJECT_SOURCE_DIR}/client/client.cpp"
)

add_executable(GraphServer
    "${PROJECT_SOURCE_DIR}/server/worker.cpp"
)