//
// Created by bytedance on 2021/8/3.
//

#include "graph_aware_cache.h"

using namespace ByteCamp;
using namespace ByteGraph;

std::shared_ptr<ByteGraph::GraphInfo> GraphAwareCache::GetFullGraphInfo() { return graph_info_; }

std::vector<std::shared_ptr<ByteGraph::NodeFeature>> GraphAwareCache::GetNodeFeature(
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

std::shared_ptr<ByteGraph::Neighbor> GraphAwareCache::GetNeighbors(const ByteGraph::NodeId &nodeId,
                                                                   const ByteGraph::EdgeType &edgeType) {
    if (!node_neighbors_cache_.count(nodeId) || !node_neighbors_cache_[nodeId].count(edgeType)) {
        return nullptr;
    }
    return node_neighbors_cache_[nodeId][edgeType];
}

void GraphAwareCache::PutFullGraphInfo(const ByteGraph::GraphInfo &graphInfo) {
    graph_info_ = std::make_shared<ByteGraph::GraphInfo>(graphInfo);
}

void GraphAwareCache::PutNodeFeature(const ByteGraph::NodeId &nodeId, const ByteGraph::NodeFeature &nodeFeature) {
    assert(node_feature_cache_.size() <= node_feature_cache_upper_num_);
    if (node_feature_cache_.size() == node_feature_cache_upper_num_) {
        // pick node with least out-degree
        auto delNodeId = node_feature_pq_.top();
        node_feature_pq_.pop();
        node_feature_cache_.erase(delNodeId);
    }
    node_feature_cache_[nodeId] = std::make_shared<NodeFeature>(nodeFeature);
}

void GraphAwareCache::PutNodeNeighbors(const ByteGraph::NodeId &nodeId, const ByteGraph::EdgeType &edgeType,
                                       const ByteGraph::Neighbor &neighborNodes) {assert(node_neighbors_cache_.size() <= node_neighbors_cache_upper_num_);
    if (node_neighbors_cache_.size() == node_neighbors_cache_upper_num_) {
        // pick node with least out-degree
        auto delNodeId = node_neighbors_pq_.top();
        node_neighbors_pq_.pop();
        node_neighbors_cache_.erase(delNodeId);
    }
    node_neighbors_cache_[nodeId][edgeType] = std::make_shared<Neighbor>(neighborNodes);
}

void GraphAwareCache::DelFullGraphInfo() {
    graph_info_.reset();
}

void GraphAwareCache::DelNodeFeature(const ByteGraph::NodeId &nodeId) {
    if (node_feature_cache_.count(nodeId)) node_feature_cache_[nodeId].reset();
}

void GraphAwareCache::DelNodeNeighbors(const ByteGraph::NodeId &nodeId, const ByteGraph::EdgeType &edgeType) {
    if (node_neighbors_cache_.count(nodeId) && node_neighbors_cache_[nodeId].count(edgeType))
        node_neighbors_cache_[nodeId][edgeType].reset();
}

std::shared_ptr<Cache> NewGraphAwareCache(size_t capacity, std::function<bool(ByteGraph::NodeId, ByteGraph::NodeId)> cmp) {
    return std::static_pointer_cast<Cache>(std::make_shared<GraphAwareCache>(capacity, cmp));
}