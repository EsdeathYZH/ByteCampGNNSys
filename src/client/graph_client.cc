#include <glog/logging.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TSocket.h>

#include "client_without_cache.h"

using std::string;
using namespace ::ByteGraph;
using namespace ByteCamp;

int main(int argc, char **argv) {
    // Initialize Google’s logging library.
    google::InitGoogleLogging(argv[0]);
    google::SetLogDestination(google::INFO, "/tmp/log/INFO_");
    // google::SetStderrLogging(google::INFO);
    FLAGS_logtostderr = true;

    string peerIP = "127.0.0.1";
    int port = 9090;
    std::vector<std::pair<string, int>> servers{std::make_pair(peerIP, port)};
    auto client = std::make_shared<ClientWithoutCache>(servers);

    GraphInfo graphInfo;
    client->GetFullGraphInfo(graphInfo);
    LOG(INFO) << "GetFullGraphInfo";
    LOG(INFO) << "num_papers:" << graphInfo.infos_[0];
    LOG(INFO) << "num_authors:" << graphInfo.infos_[1];
    LOG(INFO) << "num_institutions:" << graphInfo.infos_[2];
    LOG(INFO) << "feature_dim:" << graphInfo.infos_[3];
    LOG(INFO) << "num_classes:" << graphInfo.infos_[4];
    BatchNodes batchNodes;
    client->SampleBatchNodes(1, 16, SampleStrategy::RANDOM, batchNodes);
    LOG(INFO) << "SampleBatchNodes";
    for(int i = 0 ; i < 4; i++) {
        LOG(INFO) << batchNodes.node_ids[i];
    }
    std::vector<NodeFeature> r;
    client->GetNodeFeature({3}, 7, r);
    LOG(INFO) << "GetNodeFeature";
    for(int i = 0 ; i < 4; i++) {
        LOG(INFO) << ((float*)r[0].data())[i];
    }
    // FeatureType featureType;
    // std::vector<IDFeaturePair> neighbors;
    // client->GetNeighborsWithFeature(1, 2, featureType, neighbors);
    // LOG(INFO) << "GetNeighborsWithFeature";
    // std::vector<NodeId> nodes;
    // client->RandomWalk(1024, 2, nodes);
    // LOG(INFO) << "RandomWalk";
    return 0;
}