//
// Created by bytedance on 2021/8/3.
//

#ifndef BYTEGRAPH_GRAPH_AWARE_CACHE_H
#define BYTEGRAPH_GRAPH_AWARE_CACHE_H

#include <functional>
#include <queue>
#include <unordered_map>
#include <vector>

#include "cache_base.h"

namespace ByteCamp {

std::shared_ptr<Cache> NewGraphAwareCache(size_t capacity);

class GraphAwareCache : public Cache {
   public:
    const int32_t NODE_FEATURE_CACHE_SPLIT = 4500;
    const int32_t NODE_NEIGHBORS_CACHE_SPLIT = 4500;

   public:
    explicit GraphAwareCache(size_t capacity, const std::function<bool(ByteGraph::NodeId, ByteGraph::NodeId)>& cmp)
        : graph_info_(nullptr)
        , capacity_(capacity)
        , node_feature_cache_upper_num_(capacity_ / NODE_FEATURE_CACHE_SPLIT)
        , node_feature_pq_(cmp)
        , node_neighbors_cache_upper_num_(capacity_ / NODE_NEIGHBORS_CACHE_SPLIT)
        , node_neighbors_pq_(cmp) {}

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
    std::shared_ptr<ByteGraph::GraphInfo> graph_info_;
    const size_t capacity_;

    const size_t node_feature_cache_upper_num_;
    std::unordered_map<ByteGraph::NodeId, std::shared_ptr<ByteGraph::NodeFeature>> node_feature_cache_;
    std::priority_queue<ByteGraph::NodeId, std::vector<ByteGraph::NodeId>,
                        std::function<bool(ByteGraph::NodeId, ByteGraph::NodeId)>>
        node_feature_pq_;

    const size_t node_neighbors_cache_upper_num_;
    std::unordered_map<ByteGraph::NodeId, std::unordered_map<ByteGraph::EdgeType, std::shared_ptr<ByteGraph::Neighbor>>>
        node_neighbors_cache_;
    std::priority_queue<ByteGraph::NodeId, std::vector<ByteGraph::NodeId>,
                        std::function<bool(ByteGraph::NodeId, ByteGraph::NodeId)>>
        node_neighbors_pq_;
};

}  // namespace ByteCamp

#endif  // BYTEGRAPH_GRAPH_AWARE_CACHE_H
