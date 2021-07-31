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
    auto client = std::make_shared<ClientWithoutCache>(peerIP, port);

    GraphInfo graphInfo;
    client->GetFullGraphInfo(graphInfo);
    LOG(INFO) << "GetFullGraphInfo";
    BatchNodes batchNodes;
    client->SampleBatchNodes(1, 1024, SampleStrategy::ALIAS, batchNodes);
    LOG(INFO) << "SampleBatchNodes";
    NodeFeature r;
    client->GetNodeFeature(14, {22}, r);
    LOG(INFO) << "GetNodeFeature";
    std::vector<FeatureType> featureTypes;
    std::vector<IDFeaturePair> neighbors;
    client->GetNeighborsWithFeature(1, 2, featureTypes, neighbors);
    LOG(INFO) << "GetNeighborsWithFeature";
    std::vector<NodeId> nodes;
    client->RandomWalk(1024, 2, nodes);
    LOG(INFO) << "RandomWalk";
    return 0;
}
