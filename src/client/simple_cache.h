#ifndef BYTEGRAPH_SIMPLE_CACHE_H
#define BYTEGRAPH_SIMPLE_CACHE_H

#include <unordered_map>

#include "cache_base.h"

namespace ByteCamp {

std::shared_ptr<Cache> NewSimpleCache(size_t capacity);

class SimpleCache : public Cache {
   public:
    explicit SimpleCache(size_t capacity) : graph_info_(nullptr), capacity_(capacity), usage_(0) {}

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

    // return a an estimate of the combined charges
    // of all elements stored in the cache.
    size_t GetTotalCharge() const override { return capacity_; }

   private:
    std::shared_ptr<ByteGraph::GraphInfo> graph_info_;
    const size_t capacity_;
    size_t usage_;
    std::unordered_map<ByteGraph::NodeId, std::shared_ptr<ByteGraph::NodeFeature>> node_feature_cache_;
    std::unordered_map<ByteGraph::NodeId, std::unordered_map<ByteGraph::EdgeType, std::shared_ptr<ByteGraph::Neighbor>>>
        node_neighbors_cache_;
};

}  // namespace ByteCamp

#endif  // BYTEGRAPH_SIMPLE_CACHE_H
