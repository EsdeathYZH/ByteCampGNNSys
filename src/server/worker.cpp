#include <glog/logging.h>
#include "worker.h"

#include <iostream>

int main(int argc, char** argv) {
    // Initialize Google’s logging library.
    google::InitGoogleLogging(argv[0]);
    google::SetLogDestination(google::INFO, "/tmp/log/INFO_");
    int worker_idx = std::atoi(argv[1]);
    std::cout << "[Server cout] hello worker" << worker_idx << "\n";
    int port = 9090;
    ::std::shared_ptr<Byte::Graph> graph = std::make_shared<Byte::Graph>(worker_idx, "/dataset");
    ::std::shared_ptr<Byte::GraphEngine> engine(new Byte::GraphEngine(graph));
    ::std::shared_ptr<GraphServicesHandler> handler(new GraphServicesHandler(engine));
    ::std::shared_ptr<at::server::TProcessor> processor(new ByteGraph::GraphServicesProcessor(handler));

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

void GraphServicesHandler::getFullGraphInfo(ByteGraph::GraphInfo& _return) {
    // Your implementation goes here
    printf("[Server printf] getFullGraphInfo\n");
    Byte::GraphMeta meta = engine_->getGraphInfo();
    _return.infos_.push_back(meta.num_papers);
    _return.infos_.push_back(meta.num_authors);
    _return.infos_.push_back(meta.num_institutions);
    _return.infos_.push_back(meta.paper_feat_dim);
    _return.infos_.push_back(meta.num_classes);
}

void GraphServicesHandler::SampleBatchNodes(ByteGraph::BatchNodes& _return, 
                                            const ByteGraph::NodeType type, 
                                            const int32_t batch_size,
                                            const ByteGraph::SampleStrategy::type strategy) {
    // Your implementation goes here
    printf("[Server printf] SampleBatchNodes\n");
    if(strategy == ByteGraph::SampleStrategy::type::RANDOM) {
        auto nodes = engine_->sampleNodesFromRandom(type, batch_size);
        _return.node_ids.swap(nodes);
    } else {
        auto nodes = engine_->sampleNodesFromWeight(type, batch_size);
        _return.node_ids.swap(nodes);
    }
}

void GraphServicesHandler::GetNodeFeature(ByteGraph::NodeFeature& _return, 
                                          const ByteGraph::NodeId node_id,
                                          const ByteGraph::FeatureType feat_type) {
    // Your implementation goes here
    printf("[Server printf] GetNodeFeature\n");
    Byte::Feature feat = engine_->getNodeFeature(node_id, feat_type);
    _return.resize(feat.sz);
    if(feat.stride == 1) {
        memcpy(_return.data(), feat.data, feat.sz * sizeof(Byte::FeatureData));
    } else {
        for(int i = 0; i < feat.sz; i++) {
            memcpy(_return.data()+i, feat.data+feat.stride*i, sizeof(Byte::FeatureData));
        }
    }
}

void GraphServicesHandler::GetNodeNeighbors(ByteGraph::Neighbor& _return, 
                                            const ByteGraph::NodeId node_id, 
                                            const ByteGraph::EdgeType edge_type) {
    // Your implementation goes here
    printf("[Server printf] GetNodeNeighbors\n");
    Byte::NodeList nodes = engine_->getNodeNeighbors(node_id, edge_type);
    _return.resize(nodes.sz);
    std::copy(nodes.data, nodes.data + nodes.sz, std::begin(_return));
}

void GraphServicesHandler::GetNeighborsWithFeature(std::vector<ByteGraph::IDFeaturePair>& _return, 
                                                   const ByteGraph::NodeId node_id,
                                                   const ByteGraph::EdgeType edge_type,
                                                   const ByteGraph::FeatureType feat_type) {
    // Your implementation goes here
    printf("[Server printf] GetNeighborsWithFeature\n");
}

void GraphServicesHandler::SampleNeighbor(std::vector<ByteGraph::IDNeighborPair>& _return, 
                                          const int32_t batch_size,
                                          const ByteGraph::NodeType node_type, 
                                          const ByteGraph::EdgeType edge_type,
                                          const int32_t sample_num) {
    // Your implementation goes here
    printf("[Server printf] SampleNeighbor\n");
}

void GraphServicesHandler::RandomWalk(std::vector<ByteGraph::NodeId>& _return, const int32_t batch_size, const int32_t walk_len) {
    // Your implementation goes here
    printf("[Server printf] RandomWalk\n");
}
