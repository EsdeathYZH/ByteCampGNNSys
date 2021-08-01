#include "client_with_cache.h"

#include <glog/logging.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TSocket.h>

#include "cache_base.h"

using std::string;

namespace at = ::apache::thrift;
namespace atp = ::apache::thrift::protocol;
namespace att = ::apache::thrift::transport;

using namespace ::ByteGraph;
using namespace ::ByteCamp;

ClientWithCache::ClientWithCache(std::string peerIP, int port, std::shared_ptr<Cache> cache)
    : socket_(std::make_shared<att::TSocket>(std::move(peerIP), port))
    , transport_(std::static_pointer_cast<att::TTransport>(std::make_shared<att::TFramedTransport>(socket_)))
    , protocol_(std::static_pointer_cast<atp::TProtocol>(std::make_shared<atp::TBinaryProtocol>(transport_)))
    , rpc_client_(std::make_shared<ByteGraph::GraphServicesClient>(protocol_))
    , cache_(std::move(cache)) {
    // open connection, may throw exception.
    transport_->open();
}

void ClientWithCache::GetFullGraphInfo(ByteGraph::GraphInfo &graphInfo) {
    auto graphInfoPtr = cache_->GetFullGraphInfo();
    if (nullptr != graphInfoPtr) {
        graphInfo = *graphInfoPtr;
        return;
    }
    rpc_client_->getFullGraphInfo(graphInfo);
    cache_->PutFullGraphInfo(graphInfo);
}

void ClientWithCache::SampleBatchNodes(const ByteGraph::NodeType &type, const int32_t &batchSize,
                                       const ByteGraph::SampleStrategy::type &sampleStrategy,
                                       ByteGraph::BatchNodes &batchNodes) {
    rpc_client_->SampleBatchNodes(batchNodes, type, batchSize, sampleStrategy);
}

void ClientWithCache::GetNodeFeature(const std::vector<ByteGraph::NodeId> &nodes,
                                     const ByteGraph::FeatureType &featureType, ByteGraph::NodesFeature &nodesFeature) {
    auto nodesFeaturePtr = cache_->GetNodeFeature(nodes);
    auto size = nodes.size();
    std::vector<NodeId> notInCacheNodes;
    for (size_t i = 0; i < size; ++i) {
        if (nodesFeaturePtr[i] == nullptr) {
            notInCacheNodes.push_back(nodes[i]);
        }
    }
    NodesFeature notInCacheNodesFeature;
    rpc_client_->GetNodeFeature(notInCacheNodesFeature, notInCacheNodes, featureType);
    assert(notInCacheNodes.size() == notInCacheNodesFeature.size());
    size = notInCacheNodes.size();
    for (size_t i = 0; i < size; ++i) {
        cache_->PutNodeFeature(notInCacheNodes[i], notInCacheNodesFeature[i]);
    }
    nodesFeaturePtr = cache_->GetNodeFeature(nodes);
    size = nodes.size();
    nodesFeature.clear();
    nodesFeature.reserve(size);
    for (size_t i = 0; i < size; ++i) {
        nodesFeature.push_back(*nodesFeaturePtr[i]);
    }
}

void ClientWithCache::GetNeighborsWithFeature(const ByteGraph::NodeId &nodeId, const ByteGraph::EdgeType &edgeType,
                                              const ByteGraph::FeatureType &featureType,
                                              std::vector<ByteGraph::IDFeaturePair> &neighbors) {
    ByteGraph::Neighbor neighborNodes;
    auto neighborNodesPtr = cache_->GetNeighbors(nodeId, edgeType);
    if (nullptr == neighborNodesPtr) {
        // degrade to rpc call
        rpc_client_->GetNodeNeighbors(neighborNodes, nodeId, edgeType);
        cache_->PutNodeNeighbors(nodeId, edgeType, neighborNodes);
    }
    auto size = neighborNodes.size();
    // preallocate space
    neighbors.reserve(size);
    NodesFeature neighborNodesFeature;
    GetNodeFeature(neighborNodes, featureType, neighborNodesFeature);
    for (size_t i = 0; i < size; ++i) {
        IDFeaturePair tmp;
        tmp.node_id = neighborNodes[i];
        tmp.features.push_back(std::move(neighborNodesFeature[i]));
        neighbors.push_back(std::move(tmp));
    }
}

void ClientWithCache::SampleNeighbor(const int32_t &batchSize, const ByteGraph::NodeType &nodeType,
                                     const ByteGraph::NodeType &neighborType, const int32_t &sampleNum,
                                     std::vector<ByteGraph::IDNeighborPair> &neighbors) {
    rpc_client_->SampleNeighbor(neighbors, batchSize, nodeType, neighborType, sampleNum);
}

void ClientWithCache::RandomWalk(const int32_t &batchSize, const int32_t &walkLen,
                                 std::vector<ByteGraph::NodeId> &nodes) {
    rpc_client_->RandomWalk(nodes, batchSize, walkLen);
}
