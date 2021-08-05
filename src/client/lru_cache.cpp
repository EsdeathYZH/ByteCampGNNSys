#include "lru_cache.h"

#include <glog/logging.h>

#include <vector>

using namespace ByteGraph;

namespace ByteCamp {

LRUCache::~LRUCache() {
    LOG(INFO) << "cache nf_total cnt:" << nf_total_count_ << " nf_hit cnt:" << nf_hit_count_
              << " nf_hit_rate:" << (double) nf_hit_count_ / (double) nf_total_count_;
    LOG(INFO) << "cache nn_total cnt:" << nn_total_count_ << " nn_hit cnt:" << nn_hit_count_
              << " nn_hit_rate:" << (double) nn_hit_count_ / (double) nn_total_count_;
}

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
    ++nn_total_count_;
    if (!node_neighbors_cache_.count(nodeId) || !node_neighbors_cache_[nodeId].count(edgeType)) {
        return nullptr;
    }
    ++nn_hit_count_;
    TouchNodeNeighbors(nodeId);
    return node_neighbors_cache_[nodeId][edgeType];
}

void LRUCache::PutFullGraphInfo(const ByteGraph::GraphInfo &graphInfo) {
    graph_info_ = std::make_shared<GraphInfo>(graphInfo);
}

void LRUCache::PutNodeFeature(const ByteGraph::NodeId &nodeId, const ByteGraph::NodeFeature &nodeFeature) {
    static auto ptr = std::make_shared<NodeFeature>(768, 0);
    if (node_feature_cache_.count(nodeId)) {
        TouchNodeFeature(nodeId);
    } else {
        if (node_feature_cache_.size() == node_feature_cache_upper_num_) {
            node_feature_cache_.erase(node_feature_elements_.back());
            node_feature_elements_.pop_back();
        }
        node_feature_elements_.push_front(nodeId);
    }
    node_feature_cache_[nodeId] = {ptr, node_feature_elements_.begin()};
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
    ++nf_total_count_;
    if (!node_feature_cache_.count(nodeId)) {
        return nullptr;
    }
    ++nf_hit_count_;
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

}  // namespace ByteCamp