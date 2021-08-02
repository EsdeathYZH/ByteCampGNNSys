#include "client_without_cache.h"

#include <glog/logging.h>
#include "rpc_client.h"

using std::string;

using namespace ::ByteGraph;
using namespace ::ByteCamp;

ClientWithoutCache::ClientWithoutCache(std::string peerIP, int port)
    : rpc_client_(std::make_shared<RpcClient>(peerIP, port)) {
    LOG(INFO) << "connect to server:" << peerIP << " port:" << port;
}

void ClientWithoutCache::GetFullGraphInfo(ByteGraph::GraphInfo &graphInfo) { rpc_client_->GetFullGraphInfo(graphInfo); }

void ClientWithoutCache::SampleBatchNodes(const ByteGraph::NodeType &type, const int32_t &batchSize,
                                          const ByteGraph::SampleStrategy::type &sampleStrategy,
                                          ByteGraph::BatchNodes &batchNodes) {
    rpc_client_->SampleBatchNodes(type, batchSize, sampleStrategy, batchNodes);
}

void ClientWithoutCache::GetNodeFeature(const std::vector<ByteGraph::NodeId> &nodes,
                                        const ByteGraph::FeatureType &featureType,
                                        ByteGraph::NodesFeature &nodeFeature) {
    rpc_client_->GetNodeFeature(nodes, featureType, nodeFeature);
}

void ClientWithoutCache::GetNeighborsWithFeature(const ByteGraph::NodeId &nodeId,
                                                 const ByteGraph::EdgeType &neighborType,
                                                 const ByteGraph::FeatureType &featureType,
                                                 std::vector<ByteGraph::IDFeaturePair> &neighbors) {
    rpc_client_->GetNeighborsWithFeature(nodeId, neighborType, featureType, neighbors);
}

void ClientWithoutCache::SampleNeighbor(const int32_t &batchSize, const ByteGraph::NodeType &nodeType,
                                        const ByteGraph::NodeType &neighborType, const int32_t &sampleNum,
                                        std::vector<ByteGraph::IDNeighborPair> &neighbors) {
    rpc_client_->SampleNeighbor(batchSize, nodeType, neighborType, sampleNum, neighbors);
}

void ClientWithoutCache::RandomWalk(const int32_t &batchSize, const int32_t &walkLen,
                                    std::vector<ByteGraph::NodeId> &nodes) {
    rpc_client_->RandomWalk(batchSize, walkLen, nodes);
}
