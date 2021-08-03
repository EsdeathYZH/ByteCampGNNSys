#include <glog/logging.h>
#include "worker.h"

#include <iostream>

static void
usage(char *fn)
{
    std::cout << "usage: " << fn << " <partition_idx> [options]" << std::endl;
    std::cout << "options:" << std::endl;
}

int main(int argc, char** argv) {
    // Initialize Google’s logging library.
    google::InitGoogleLogging(argv[0]);
    google::SetLogDestination(google::INFO, "/tmp/log/INFO_");

    if (argc < 2) {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    int worker_idx = std::atoi(argv[1]);
    std::cout << "[Server cout] hello worker" << worker_idx << "\n";
    int port = 9090;
    ::std::shared_ptr<Byte::Graph> graph = std::make_shared<Byte::Graph>(worker_idx, "/data");
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
    LOG(INFO) << "[Server printf] sayHello";
    _return = std::string("I am worker ") + std::to_string(workerId) + std::string(", I want to say: ") + content +
              std::string(".");
}

void GraphServicesHandler::getFullGraphInfo(ByteGraph::GraphInfo& _return) {
    LOG(INFO) << "[Server printf] getFullGraphInfo";
    Byte::GraphMeta meta = engine_->getGraphInfo();
    _return.infos_.push_back(meta.num_papers);
    _return.infos_.push_back(meta.num_authors);
    _return.infos_.push_back(meta.num_institutions);
    _return.infos_.push_back(meta.paper_feat_dim);
    _return.infos_.push_back(meta.num_classes);

    _return.total_weights_.assign(std::begin(meta.sum_weights), std::end(meta.sum_weights));
}

void GraphServicesHandler::SampleBatchNodes(ByteGraph::BatchNodes& _return, 
                                            const ByteGraph::NodeType type, 
                                            const int32_t batch_size,
                                            const ByteGraph::SampleStrategy::type strategy) {
    LOG(INFO) << "[Server printf] SampleBatchNodes";
    if(strategy == ByteGraph::SampleStrategy::type::RANDOM) {
        auto nodes = engine_->sampleNodesFromRandom(type, batch_size);
        _return.node_ids.swap(nodes);
    } else {
        auto nodes = engine_->sampleNodesFromWeight(type, batch_size);
        _return.node_ids.swap(nodes);
    }
}

void GraphServicesHandler::GetNodeFeature(std::vector<ByteGraph::NodeFeature>& _return, 
                                          const std::vector<ByteGraph::NodeId>& nodes,
                                          const ByteGraph::FeatureType feat_type) {
    LOG(INFO) << "[Server printf] GetNodeFeature";
    _return.resize(nodes.size());
    for(int i = 0; i < nodes.size(); i++) {
        Byte::Feature feat = engine_->getNodeFeature(nodes[i], feat_type);
        _return[i].resize(feat.sz);
        if(feat.stride == 1) {
            memcpy(_return[i].data(), feat.data, feat.sz * sizeof(Byte::FeatureData));
        } else {
            for(int j = 0; j < feat.sz; j++) {
                memcpy(_return[i].data()+j, feat.data+feat.stride*j, sizeof(Byte::FeatureData));
            }
        }
    }
}

void GraphServicesHandler::GetNodeNeighbors(std::vector<ByteGraph::Neighbor>& _return, 
                                            const std::vector<ByteGraph::NodeId>& nodes, 
                                            const ByteGraph::EdgeType edge_type) {
    LOG(INFO) << "[Server printf] GetNodeNeighbors";
    _return.resize(nodes.size());
    for(int idx = 0; idx < nodes.size(); idx++) {
        Byte::NodeID node_id = nodes[idx];
        Byte::NodeList neighbors = engine_->getNodeNeighbors(node_id, edge_type);
        _return[idx].assign(neighbors.data, neighbors.data + neighbors.sz);
    }
}

void GraphServicesHandler::GetNeighborsWithFeature(std::vector<ByteGraph::IDFeaturePair>& _return, 
                                                   const ByteGraph::NodeId node_id,
                                                   const ByteGraph::EdgeType edge_type,
                                                   const ByteGraph::FeatureType feat_type) {
    LOG(INFO) << "[Server printf] GetNeighborsWithFeature";
}

void GraphServicesHandler::GetNodeWeights(std::vector<int32_t>& _return, 
                                          const std::vector<ByteGraph::NodeId>& nodes, 
                                          const ByteGraph::FeatureType feat_type,
                                          const int32_t feat_idx) {
    LOG(INFO) << "[Server printf] GetNodeWeights";
    _return.resize(nodes.size());
    for(int idx = 0; idx < nodes.size(); idx++) {
        _return[idx] = engine_->getNodeFeatureItem(nodes[idx], feat_type, feat_idx);
    }
}

void GraphServicesHandler::SampleNodeNeighbors(std::vector<std::vector<ByteGraph::NodeId>>& _return, 
                                          const std::vector<ByteGraph::NodeId>& nodes, 
                                          const ByteGraph::EdgeType edge_type,
                                          const int32_t sample_num) {
    LOG(INFO) << "[Server printf] SampleNeighbor";
    _return.resize(nodes.size());
    for(int idx = 0; idx < nodes.size(); idx++) {
        Byte::NodeID node_id = nodes[idx];
        _return[idx] = engine_->sampleNodeNeighborsFromRandom(node_id, edge_type, sample_num);
    }
}

void GraphServicesHandler::RandomWalk(std::vector<ByteGraph::NodeId>& _return, const int32_t batch_size, const int32_t walk_len) {
    // Your implementation goes here
    LOG(INFO) << "[Server printf] RandomWalk";
}
