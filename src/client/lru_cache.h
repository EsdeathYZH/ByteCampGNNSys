#ifndef BYTEGRAPH_LRU_CACHE_H
#define BYTEGRAPH_LRU_CACHE_H

#include <list>
#include <unordered_map>

#include "cache_base.h"

namespace ByteCamp {

std::shared_ptr<Cache> NewLRUCache(size_t capacity);

class LRUCache : public Cache {
   public:
    const int32_t NODE_FEATURE_CACHE_SPLIT = 4500;
    const int32_t NODE_NEIGHBORS_CACHE_SPLIT = 4500;

   public:
    explicit LRUCache(size_t capacity)
        : graph_info_(nullptr)
        , capacity_(capacity)
        , node_feature_cache_upper_num_(capacity_ / NODE_FEATURE_CACHE_SPLIT)
        , node_neighbors_cache_upper_num_(capacity_ / NODE_NEIGHBORS_CACHE_SPLIT) {}

    std::shared_ptr<ByteGraph::GraphInfo> GetFullGraphInfo() override;

    std::vector<std::shared_ptr<ByteGraph::NodeFeature>> GetNodeFeature(
        const std::vector<ByteGraph::NodeId>& nodeIds) override;

    std::shared_ptr<ByteGraph::Neighbor> GetNeighbors(const ByteGraph::NodeId& nodeId,
                                                      const ByteGraph::EdgeType& edgeType) override;

    void PutFullGraphInfo(const ByteGraph::GraphInfo& graphInfo) override;

    void PutNodeFeature(const ByteGraph::NodeId& nodeId, const ByteGraph::NodeFeature& nodeFeature) override;

    void PutNodeNeighbors(const ByteGraph::NodeId& nodeId, const ByteGraph::EdgeType& edgeType,
                          const ByteGraph::Neighbor& neighborNodes) override;

    void DelFullGraphInfo() override;

    void DelNodeFeature(const ByteGraph::NodeId& nodeId) override;

    void DelNodeNeighbors(const ByteGraph::NodeId& nodeId, const ByteGraph::EdgeType& edgeType) override;

   private:
    std::shared_ptr<ByteGraph::NodeFeature> GetSingleNodeFeature(const ByteGraph::NodeId& nodeId);

    void TouchNodeFeature(const ByteGraph::NodeId& nodeId);

    void TouchNodeNeighbors(const ByteGraph::NodeId& nodeId);

   private:
    std::shared_ptr<ByteGraph::GraphInfo> graph_info_;
    const size_t capacity_;

    const size_t node_feature_cache_upper_num_;
    std::list<ByteGraph::NodeId> node_feature_elements_;

    using node_feature_iter = std::list<ByteGraph::NodeId>::iterator;
    using node_feature_value_type = std::pair<std::shared_ptr<ByteGraph::NodeFeature>, node_feature_iter>;
    std::unordered_map<ByteGraph::NodeId, node_feature_value_type> node_feature_cache_;

    const size_t node_neighbors_cache_upper_num_;
    std::list<ByteGraph::NodeId> node_neighbors_elements_;
    using node_neighbors_iter = std::list<ByteGraph::NodeId>::iterator;
    std::unordered_map<ByteGraph::NodeId, std::unordered_map<ByteGraph::EdgeType, std::shared_ptr<ByteGraph::Neighbor>>> node_neighbors_cache_;
    std::unordered_map<ByteGraph::NodeId, node_neighbors_iter> node_neighbors_lru_helper_;
};

}  // namespace ByteCamp

#endif  // BYTEGRAPH_LRU_CACHE_H
