#ifndef BYTEGRAPH_SIMPLE_CACHE_H
#define BYTEGRAPH_SIMPLE_CACHE_H

#include <unordered_map>

#include "cache_base.h"

namespace ByteCamp {

std::shared_ptr<Cache> NewSimpleCache(size_t capacity);

class SimpleCache : public Cache {
   public:
    const int32_t NODE_FEATURE_CACHE_SPLIT = 4500;
    const int32_t NODE_NEIGHBORS_CACHE_SPLIT  = 4500;

    explicit SimpleCache(size_t capacity)
        : graph_info_(nullptr)
        , capacity_(capacity)
        , node_feature_cache_upper_num_(capacity_ / NODE_FEATURE_CACHE_SPLIT)
        , node_neighbors_cache_upper_num_(capacity_ / NODE_NEIGHBORS_CACHE_SPLIT) {}

    ~SimpleCache();

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

    // return an estimate of the combined charges
    // of all elements stored in the cache.
    size_t GetTotalCharge() const override { return capacity_; }

   private:
    int64_t nf_hit_count_{0};
    int64_t nf_total_count_{0};
    int64_t nn_hit_count_{0};
    int64_t nn_total_count_{0};
    std::shared_ptr<ByteGraph::GraphInfo> graph_info_;
    const size_t capacity_;
    const size_t node_feature_cache_upper_num_;
    std::unordered_map<ByteGraph::NodeId, std::shared_ptr<ByteGraph::NodeFeature>> node_feature_cache_;
    const size_t node_neighbors_cache_upper_num_;
    std::unordered_map<ByteGraph::NodeId, std::unordered_map<ByteGraph::EdgeType, std::shared_ptr<ByteGraph::Neighbor>>>
        node_neighbors_cache_;
};

}  // namespace ByteCamp

#endif  // BYTEGRAPH_SIMPLE_CACHE_H
