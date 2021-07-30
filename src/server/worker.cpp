#include "worker.h"

#include <iostream>

int main(int argc, char** argv) {
    std::cout << "[Server cout] hello worker\n";
    int port = 9090;
    ::std::shared_ptr<Byte::Graph> graph = std::make_shared<Byte::Graph>("/dataset");
    ::std::shared_ptr<Byte::GraphEngine> engine(new Byte::GraphEngine(graph));
    ::std::shared_ptr<GraphServicesHandler> handler(new GraphServicesHandler(engine));
    ::std::shared_ptr<at::server::TProcessor> processor(new GraphServicesProcessor(handler));

    ::std::shared_ptr<att::TNonblockingServerSocket> serverTransport(new att::TNonblockingServerSocket(port));
    ::std::shared_ptr<atp::TProtocolFactory> protocolFactory(new atp::TBinaryProtocolFactory());

    at::server::TNonblockingServer server(processor, protocolFactory, serverTransport);
    server.serve();
    return 0;
}

GraphServicesHandler::GraphServicesHandler(std::shared_ptr<Byte::GraphEngine> engine)
    : engine_(engine) {}

void GraphServicesHandler::sayHello(std::string& _return, const int32_t workerId, const std::string& content) {
    // Your implementation goes here
    printf("[Server printf] sayHello\n");
    _return = std::string("I am worker ") + std::to_string(workerId) + std::string(", I want to say: ") + content +
              std::string(".");
}

void GraphServicesHandler::getFullGraphInfo(GraphInfo& _return) {
    // Your implementation goes here
    printf("[Server printf] getFullGraphInfo\n");
    Byte::GraphMeta meta = engine_->getGraphInfo();
    uint32_t num_papers;
    uint32_t num_authors;
    uint32_t num_institutions;
    uint32_t paper_feat_dim;
    uint32_t num_classes;
    _return.infos_.push_back(meta.num_papers);
    _return.infos_.push_back(meta.num_authors);
    _return.infos_.push_back(meta.num_institutions);
    _return.infos_.push_back(meta.num_papers);
    _return.infos_.push_back(meta.num_papers);
}

void GraphServicesHandler::SampleBatchNodes(BatchNodes& _return, const NodeType type, const int32_t batch_size,
                                            const SampleStrategy::type strategy) {
    // Your implementation goes here
    printf("[Server printf] SampleBatchNodes\n");
}

void GraphServicesHandler::GetNodeFeature(NodeFeature& _return, const NodeId node_id,
                                          const std::vector<FeatureType>& feature_id) {
    // Your implementation goes here
    printf("[Server printf] GetNodeFeature\n");
    _return = {(int32_t) node_id, (int32_t) feature_id[0]};
}

void GraphServicesHandler::GetNodeNeighbors(Neighbor& _return, const NodeId node_id, const EdgeType edge_type) {
    // Your implementation goes here
    printf("[Server printf] GetNodeNeighbors\n");
}

void GraphServicesHandler::GetNeighborsWithFeature(std::vector<IDFeaturePair>& _return, const NodeId node_id,
                                                   const EdgeType neighbor_type,
                                                   const std::vector<FeatureType>& feature_types) {
    // Your implementation goes here
    printf("[Server printf] GetNeighborsWithFeature\n");
}

void GraphServicesHandler::SampleNeighbor(std::vector<IDNeighborPair>& _return, const int32_t batch_size,
                                          const NodeType node_type, const NodeType neighbor_type,
                                          const int32_t sample_num) {
    // Your implementation goes here
    printf("[Server printf] SampleNeighbor\n");
}

void GraphServicesHandler::RandomWalk(std::vector<NodeId>& _return, const int32_t batch_size, const int32_t walk_len) {
    // Your implementation goes here
    printf("[Server printf] RandomWalk\n");
}
