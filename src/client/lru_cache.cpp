#include "lru_cache.h"

#include <vector>

#include <glog/logging.h>

using namespace ByteGraph;

namespace ByteCamp {

std::shared_ptr<ByteGraph::GraphInfo> LRUCache::GetFullGraphInfo() { return graph_info_; }

std::vector<std::shared_ptr<ByteGraph::NodeFeature>> LRUCache::GetNodeFeature(
    const std::vector<ByteGraph::NodeId> &nodeIds) {
    std::vector<std::shared_ptr<NodeFeature>> ans;
    ans.reserve(nodeIds.size());
    for (const auto &nodeId : nodeIds) {
        ans.push_back(std::move(GetSingleNodeFeature(nodeId)));
    }
    return ans;
}

std::shared_ptr<ByteGraph::Neighbor> LRUCache::GetNeighbors(const ByteGraph::NodeId &nodeId,
                                                            const ByteGraph::EdgeType &edgeType) {
    if (!node_neighbors_cache_.count(nodeId) || !node_neighbors_cache_[nodeId].count(edgeType)) {
        return nullptr;
    }
    TouchNodeNeighbors(nodeId);
    return node_neighbors_cache_[nodeId][edgeType];
}

void LRUCache::PutFullGraphInfo(const ByteGraph::GraphInfo &graphInfo) {
    graph_info_ = std::make_shared<GraphInfo>(graphInfo);
}

void LRUCache::PutNodeFeature(const ByteGraph::NodeId &nodeId, const ByteGraph::NodeFeature &nodeFeature) {
    if (node_feature_cache_.count(nodeId)) {
        TouchNodeFeature(nodeId);
    } else {
        if (node_feature_cache_.size() == node_feature_cache_upper_num_) {
            node_feature_cache_.erase(node_feature_elements_.back());
            node_feature_elements_.pop_back();
        }
        node_feature_elements_.push_front(nodeId);
    }
    node_feature_cache_[nodeId] = {std::make_shared<NodeFeature>(nodeFeature), node_feature_elements_.begin()};
}

void LRUCache::PutNodeNeighbors(const ByteGraph::NodeId &nodeId, const ByteGraph::EdgeType &edgeType,
                                const ByteGraph::Neighbor &neighborNodes) {
    if (node_neighbors_cache_.count(nodeId)) {
        TouchNodeNeighbors(nodeId);
    } else {
        if (node_neighbors_cache_.size() == node_neighbors_cache_upper_num_) {
            // delete least recently used
            node_neighbors_cache_.erase(node_neighbors_elements_.back());
            node_neighbors_lru_helper_.erase(node_neighbors_elements_.back());
            node_neighbors_elements_.pop_back();
        }
        node_neighbors_elements_.push_front(nodeId);
        node_neighbors_lru_helper_[nodeId] = node_neighbors_elements_.begin();
    }
    node_neighbors_cache_[nodeId][edgeType] = std::make_shared<Neighbor>(neighborNodes);
}

void LRUCache::DelFullGraphInfo() { graph_info_.reset(); }

void LRUCache::DelNodeFeature(const ByteGraph::NodeId &nodeId) {
    if (node_feature_cache_.count(nodeId)) {
        node_feature_elements_.erase(node_feature_cache_[nodeId].second);
        node_feature_cache_.erase(nodeId);
    }
}

void LRUCache::DelNodeNeighbors(const ByteGraph::NodeId &nodeId, const ByteGraph::EdgeType &edgeType) {
    LOG(WARNING) << "lru cache current not support delete node neighbors";
}

std::shared_ptr<ByteGraph::NodeFeature> LRUCache::GetSingleNodeFeature(const ByteGraph::NodeId &nodeId) {
    if (!node_feature_cache_.count(nodeId)) {
        return nullptr;
    }
    TouchNodeFeature(nodeId);
    return node_feature_cache_[nodeId].first;
}

void LRUCache::TouchNodeFeature(const ByteGraph::NodeId &nodeId) {
    auto iter = node_feature_cache_.find(nodeId);
    node_feature_elements_.erase(iter->second.second);
    node_feature_elements_.push_front(nodeId);
    iter->second.second = node_feature_elements_.begin();
}

void LRUCache::TouchNodeNeighbors(const ByteGraph::NodeId &nodeId) {
    auto iter = node_neighbors_lru_helper_.find(nodeId);
    node_feature_elements_.erase(iter->second);
    node_feature_elements_.push_front(nodeId);
    iter->second = node_feature_elements_.begin();
}

std::shared_ptr<Cache> NewLRUCache(size_t capacity) {
    return std::static_pointer_cast<Cache>(std::make_shared<LRUCache>(capacity));
}

} // namespace ByteCamp