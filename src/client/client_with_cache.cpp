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
    auto cache_hit = cache_->GetFullGraphInfo(graphInfo);
    if (cache_hit) {
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

void ClientWithCache::GetNodeFeature(const ByteGraph::NodeId &nodeId,
                                     const ByteGraph::FeatureType &featureTypes,
                                     ByteGraph::NodeFeature &nodeFeature) {
    auto cache_hit = cache_->GetNodeFeature(nodeId, nodeFeature);
    // todo: current return all features, maybe
    //  should do filter according to featureType
    if (cache_hit) {
        return;
    }
    rpc_client_->GetNodeFeature(nodeFeature, nodeId, featureTypes);
    cache_->PutNodeFeature(nodeId, nodeFeature);
}

void ClientWithCache::GetNeighborsWithFeature(const ByteGraph::NodeId &nodeId, const ByteGraph::EdgeType &edgeType,
                                              const ByteGraph::FeatureType &featureTypes,
                                              std::vector<ByteGraph::IDFeaturePair> &neighbors) {
    ByteGraph::Neighbor neighborNodes;
    auto cacheHit = cache_->GetNeighbors(nodeId, edgeType, neighborNodes);
    if (!cacheHit) {
        // degrade to rpc call
        rpc_client_->GetNodeNeighbors(neighborNodes, nodeId, edgeType);
        cache_->PutNodeNeighbors(nodeId, neighborNodes);
    }
    // preallocate space
    neighbors.reserve(neighborNodes.size());
    for (const auto &node : neighborNodes) {
        ByteGraph::NodeFeature nodeFeature;
        // todo: current return all features, maybe
        //  should do filter according to featureType
        cacheHit = cache_->GetNodeFeature(node, nodeFeature);
        if (!cacheHit) {
            // degrade to rpc call
            rpc_client_->GetNodeFeature(nodeFeature, node, featureTypes);
            cache_->PutNodeFeature(node, nodeFeature);
        }
        ByteGraph::IDFeaturePair tmpPair;
        tmpPair.node_id = node;
        tmpPair.features.emplace_back(std::move(nodeFeature));
        neighbors.emplace_back(std::move(tmpPair));
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
