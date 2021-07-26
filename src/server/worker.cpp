#include "worker.h"

#include <iostream>

int main(int argc, char** argv) {
    std::cout << "[Server cout] hello worker\n";
    int port = 9090;
    ::std::shared_ptr<GraphServicesHandler> handler(new GraphServicesHandler());
    ::std::shared_ptr<at::server::TProcessor> processor(new GraphServicesProcessor(handler));

    ::std::shared_ptr<att::TNonblockingServerSocket> serverTransport(new att::TNonblockingServerSocket(port));
    ::std::shared_ptr<atp::TProtocolFactory> protocolFactory(new atp::TBinaryProtocolFactory());

    at::server::TNonblockingServer server(processor, protocolFactory, serverTransport);
    server.serve();
    return 0;
}

GraphServicesHandler::GraphServicesHandler() = default;

void GraphServicesHandler::sayHello(std::string& _return, const int32_t workerId, const std::string& content) {
    // Your implementation goes here
    printf("[Server printf] sayHello\n");
    _return = std::string("I am worker ") + std::to_string(workerId) + std::string(", I want to say: ") + content +
              std::string(".");
}
