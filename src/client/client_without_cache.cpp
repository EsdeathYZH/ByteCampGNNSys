#include "client_without_cache.h"

#include <glog/logging.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TSocket.h>

using std::string;

namespace at = ::apache::thrift;
namespace atp = ::apache::thrift::protocol;
namespace att = ::apache::thrift::transport;

using namespace ::ByteGraph;
using namespace ::ByteCamp;

ClientWithoutCache::ClientWithoutCache(std::string peerIP, int port)
    : socket_(std::make_shared<att::TSocket>(std::move(peerIP), port))
    , transport_(std::static_pointer_cast<att::TTransport>(std::make_shared<att::TFramedTransport>(socket_)))
    , protocol_(std::static_pointer_cast<atp::TProtocol>(std::make_shared<atp::TBinaryProtocol>(transport_)))
    , rpc_client_(std::make_shared<ByteGraph::GraphServicesClient>(protocol_)) {
    // open connection, may throw exception.
    transport_->open();
    LOG(INFO) << "connect to server:" << peerIP << " port:" << port;
}

void ClientWithoutCache::GetFullGraphInfo(ByteGraph::GraphInfo &graphInfo) { rpc_client_->getFullGraphInfo(graphInfo); }

void ClientWithoutCache::SampleBatchNodes(const ByteGraph::NodeType &type, const int32_t &batchSize,
                                          const ByteGraph::SampleStrategy::type &sampleStrategy,
                                          ByteGraph::BatchNodes &batchNodes) {
    rpc_client_->SampleBatchNodes(batchNodes, type, batchSize, sampleStrategy);
}

void ClientWithoutCache::GetNodeFeature(const std::vector<ByteGraph::NodeId> &nodes,
                                        const ByteGraph::FeatureType &featureType,
                                        ByteGraph::NodeFeature &nodeFeature) {
    rpc_client_->GetNodeFeature(nodeFeature, nodes, featureType);
}

void ClientWithoutCache::GetNeighborsWithFeature(const ByteGraph::NodeId &nodeId,
                                                 const ByteGraph::EdgeType &neighborType,
                                                 const ByteGraph::FeatureType &featureType,
                                                 std::vector<ByteGraph::IDFeaturePair> &neighbors) {
    rpc_client_->GetNeighborsWithFeature(neighbors, nodeId, neighborType, featureType);
}

void ClientWithoutCache::SampleNeighbor(const int32_t &batchSize, const ByteGraph::NodeType &nodeType,
                                        const ByteGraph::NodeType &neighborType, const int32_t &sampleNum,
                                        std::vector<ByteGraph::IDNeighborPair> &neighbors) {
    rpc_client_->SampleNeighbor(neighbors, batchSize, nodeType, neighborType, sampleNum);
}

void ClientWithoutCache::RandomWalk(const int32_t &batchSize, const int32_t &walkLen,
                                    std::vector<ByteGraph::NodeId> &nodes) {
    rpc_client_->RandomWalk(nodes, batchSize, walkLen);
}