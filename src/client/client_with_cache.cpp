#include "client_with_cache.h"

#include <glog/logging.h>

#include "cache_base.h"
#include "rpc_client.h"

using std::string;

using namespace ::ByteGraph;
using namespace ::ByteCamp;

ClientWithCache::ClientWithCache(const std::vector<std::pair<std::string, int>> &serverAddresses,
                                 std::shared_ptr<Cache> cache)
    : cache_(std::move(cache)) {
    rpc_clients_.reserve(serverAddresses.size());
    for (const auto &server : serverAddresses) {
        auto rpc_client = std::make_shared<RpcClient>(server.first, server.second);
        rpc_clients_.emplace_back(rpc_client);
    }
    assert(rpc_clients_.size() == serverAddresses.size());
}

void ClientWithCache::GetFullGraphInfo(ByteGraph::GraphInfo &graphInfo) {
    auto graphInfoPtr = cache_->GetFullGraphInfo();
    if (nullptr != graphInfoPtr) {
        graphInfo = *graphInfoPtr;
        return;
    }
    rpc_clients_[0]->GetFullGraphInfo(graphInfo);
    cache_->PutFullGraphInfo(graphInfo);
}

void ClientWithCache::SampleBatchNodes(const ByteGraph::NodeType &type, const int32_t &batchSize,
                                       const ByteGraph::SampleStrategy::type &sampleStrategy,
                                       ByteGraph::BatchNodes &batchNodes) {
    batchNodes.node_ids.reserve(batchSize);
    if (sampleStrategy == SampleStrategy::RANDOM) {
        int32_t size = rpc_clients_.size();
        int32_t avg = batchSize / size, last = batchSize - (avg * (size - 1));
        for (size_t i = 0; i < size; ++i) {
            ByteGraph::BatchNodes tmpBatchNodes;
            if (i == size - 1) {
                rpc_clients_[i]->SampleBatchNodes(type, last, sampleStrategy, tmpBatchNodes);
            } else {
                rpc_clients_[i]->SampleBatchNodes(type, avg, sampleStrategy, tmpBatchNodes);
            }
            batchNodes.node_ids.insert(batchNodes.node_ids.end(), tmpBatchNodes.node_ids.begin(),
                                       tmpBatchNodes.node_ids.end());
        }
    } else {
    }
}

void ClientWithCache::GetNodeFeature(const std::vector<ByteGraph::NodeId> &nodes,
                                     const ByteGraph::FeatureType &featureType, ByteGraph::NodesFeature &nodesFeature) {
    auto nodesFeaturePtr = cache_->GetNodeFeature(nodes);
    const auto size = nodes.size();
    std::vector<NodeId> notInCacheNodes;
    // check whether node feature is in cache
    for (size_t i = 0; i < size; ++i) {
        if (nodesFeaturePtr[i] == nullptr) {
            notInCacheNodes.push_back(nodes[i]);
        }
    }
    std::vector<std::vector<NodeId>> rpc_clients_nodes(size);
    for (const auto &notInCacheNode : notInCacheNodes) {
        rpc_clients_nodes[notInCacheNode % size].push_back(notInCacheNode);
    }
    for (size_t i = 0; i < size; ++i) {
        NodesFeature notInCacheNodesFeature;
        rpc_clients_[i]->GetBatchNodeFeature(rpc_clients_nodes[i], featureType, notInCacheNodesFeature);
        auto tmpSize = rpc_clients_nodes[i].size();
        assert(tmpSize == notInCacheNodesFeature.size());
        for (size_t j = 0; j < tmpSize; ++j) {
            // put node feature in cache
            cache_->PutNodeFeature(rpc_clients_nodes[i][j], nodesFeature[j]);
        }
    }
    nodesFeaturePtr = cache_->GetNodeFeature(nodes);
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
        rpc_clients_[nodeId % (rpc_clients_.size())]->GetNodeNeighbors(nodeId, edgeType, neighborNodes);
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
    // todo(liuwenjing)
    //    rpc_client_->SampleNeighbor(batchSize, nodeType, neighborType, sampleNum, neighbors);
}

void ClientWithCache::RandomWalk(const int32_t &batchSize, const int32_t &walkLen,
                                 std::vector<ByteGraph::NodeId> &nodes) {
    //    rpc_client_->RandomWalk(batchSize, walkLen, nodes);
}
