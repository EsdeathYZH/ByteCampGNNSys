#include "simple_cache.h"

using namespace ByteGraph;

namespace ByteCamp {

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
}

void SimpleCache::PutNodeFeature(const ByteGraph::NodeId &nodeId, const ByteGraph::NodeFeature &nodeFeature) {
    assert(node_feature_cache_.size() <= node_feature_cache_upper_num_);
    if (node_feature_cache_.size() == node_feature_cache_upper_num_) {
        // just pick the first element for deleting
        node_feature_cache_.erase(node_feature_cache_.begin());
    }
    node_feature_cache_[nodeId] = std::make_shared<NodeFeature>(nodeFeature);
}

void SimpleCache::PutNodeNeighbors(const ByteGraph::NodeId &nodeId, const ByteGraph::EdgeType &edgeType,
                                   const ByteGraph::Neighbor &neighborNodes) {
    assert(node_neighbors_cache_.size() <= node_neighbors_cache_upper_num_);
    if (node_neighbors_cache_.size() == node_neighbors_cache_upper_num_) {
        // just pick the first element for deleting
        node_neighbors_cache_.erase(node_neighbors_cache_.begin());
    }
    node_neighbors_cache_[nodeId][edgeType] = std::make_shared<Neighbor>(neighborNodes);
}

void SimpleCache::DelFullGraphInfo() { graph_info_.reset(); }

void SimpleCache::DelNodeFeature(const ByteGraph::NodeId &nodeId) {
    if (node_feature_cache_.count(nodeId)) node_feature_cache_[nodeId].reset();
}

void SimpleCache::DelNodeNeighbors(const ByteGraph::NodeId &nodeId, const ByteGraph::EdgeType &edgeType) {
    if (node_neighbors_cache_.count(nodeId) && node_neighbors_cache_[nodeId].count(edgeType))
        node_neighbors_cache_[nodeId][edgeType].reset();
}

std::shared_ptr<Cache> NewSimpleCache(size_t capacity) {
    return std::static_pointer_cast<Cache>(std::make_shared<SimpleCache>(capacity));
}

} // namespace ByteCamp
