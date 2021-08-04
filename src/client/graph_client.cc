#include <glog/logging.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TSocket.h>

#include "client_without_cache.h"
#include "utils/utils.h"

using std::string;
using namespace ::ByteGraph;
using namespace ByteCamp;

int main(int argc, char **argv) {
    // Initialize Google’s logging library.
    google::InitGoogleLogging(argv[0]);
    google::SetLogDestination(google::INFO, "/tmp/log/INFO_");
    // google::SetStderrLogging(google::INFO);
    FLAGS_logtostderr = true;

    const std::string fileName = "/root/configs/config.cfg";
    auto serverAddresses = ReadConfig<std::string, int>(fileName);
    std::vector<std::pair<string, int>> servers{serverAddresses};
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
    client->SampleBatchNodes(1, 16, SampleStrategy::RANDOM, 0, batchNodes);
    LOG(INFO) << "SampleBatchNodes";
    for(int i = 0 ; i < 4; i++) {
        LOG(INFO) << batchNodes.node_ids[i];
    }
    std::vector<NodeFeature> r;
    client->GetNodeFeature(batchNodes.node_ids, 7, r);
    LOG(INFO) << "GetNodeFeature";
    for(int i = 0 ; i < r.size(); i++) {
        for (int j = 0; j < r[0].size(); ++j) {
            LOG(INFO) << ((float*)r[i].data())[j];
        }
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