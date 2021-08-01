#include "simple_cache.h"

using namespace ByteCamp;
using namespace ByteGraph;

std::shared_ptr<ByteGraph::GraphInfo> SimpleCache::GetFullGraphInfo() { return graph_info_; }

std::vector<std::shared_ptr<ByteGraph::NodeFeature>> SimpleCache::GetNodeFeature(
    const std::vector<ByteGraph::NodeId> &nodeIds) {
    std::vector<std::shared_ptr<NodeFeature>> ans;
    ans.reserve(nodeIds.size());
    for (const auto &nodeId : nodeIds) {
        if (!node_feature_cache_.count(nodeId)) {
            ans.push_back(nullptr);
        } else {
            ans.push_back(node_feature_cache_[nodeId]);
        }
    }
    return ans;
}

std::shared_ptr<ByteGraph::Neighbor> SimpleCache::GetNeighbors(const ByteGraph::NodeId &nodeId,
                                                               const ByteGraph::EdgeType &edgeType) {
    if (!node_neighbors_cache_.count(nodeId) || !node_neighbors_cache_[nodeId].count(edgeType)) {
        return nullptr;
    }
    return node_neighbors_cache_[nodeId][edgeType];
}

void SimpleCache::PutFullGraphInfo(const ByteGraph::GraphInfo &graphInfo) {
    graph_info_ = std::make_shared<ByteGraph::GraphInfo>(graphInfo);
    graph_info_.reset();
}

void SimpleCache::PutNodeFeature(const ByteGraph::NodeId &nodeId, const ByteGraph::NodeFeature &nodeFeature) {
    if (node_feature_cache_.count(nodeId)) {
        assert(capacity_ >= (node_feature_cache_[nodeId]->size() * sizeof(int32_t)));
        capacity_ -= node_feature_cache_[nodeId]->size() * sizeof(int32_t);
    }
    node_feature_cache_[nodeId] = std::make_shared<NodeFeature>(nodeFeature);
    capacity_ += node_feature_cache_[nodeId]->size() * sizeof(int32_t);
}

void SimpleCache::PutNodeNeighbors(const ByteGraph::NodeId &nodeId, const ByteGraph::EdgeType &edgeType,
                                   const ByteGraph::Neighbor &neighborNodes) {
    if (node_neighbors_cache_.count(nodeId) && node_neighbors_cache_[nodeId].count(edgeType)) {
        assert(capacity_ >= node_neighbors_cache_[nodeId][edgeType]->size() * sizeof(nodeId));
        capacity_ -= node_neighbors_cache_[nodeId][edgeType]->size() * sizeof(nodeId);
    }
    node_neighbors_cache_[nodeId][edgeType] = std::make_shared<Neighbor>(neighborNodes);
    capacity_ += node_neighbors_cache_[nodeId][edgeType]->size() * sizeof(nodeId);
}

void SimpleCache::DelFullGraphInfo() { graph_info_.reset(); }

void SimpleCache::DelNodeFeature(const ByteGraph::NodeId &nodeId) {
    if (node_feature_cache_.count(nodeId)) node_feature_cache_[nodeId].reset();
}

void SimpleCache::DelNodeNeighbors(const ByteGraph::NodeId &nodeId, const ByteGraph::EdgeType &edgeType) {
    if (node_neighbors_cache_.count(nodeId) && node_neighbors_cache_[nodeId].count(edgeType))
        node_neighbors_cache_[nodeId][edgeType].reset();
}