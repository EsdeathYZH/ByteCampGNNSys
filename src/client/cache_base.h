#ifndef BYTEGRAPH_CACHE_BASE_H
#define BYTEGRAPH_CACHE_BASE_H

#include "gen-cpp/ByteGraph_types.h"

namespace ByteCamp {

class Cache {
   public:
    Cache() = default;

    Cache(const Cache&) = delete;

    Cache& operator=(const Cache&) = delete;

    virtual ~Cache() = default;

    virtual std::shared_ptr<ByteGraph::GraphInfo> GetFullGraphInfo() = 0;

    virtual std::vector<std::shared_ptr<ByteGraph::NodeFeature>> GetNodeFeature(const std::vector<ByteGraph::NodeId>& nodeIds) = 0;

    virtual std::shared_ptr<ByteGraph::Neighbor> GetNeighbors(const ByteGraph::NodeId& nodeId,
                                                              const ByteGraph::EdgeType& edgeType) = 0;

    virtual void PutFullGraphInfo(const ByteGraph::GraphInfo& graphInfo) = 0;

    virtual void PutNodeFeature(const ByteGraph::NodeId& nodeId, const ByteGraph::NodeFeature& nodeFeature) = 0;

    virtual void PutNodeNeighbors(const ByteGraph::NodeId& nodeId, const ByteGraph::EdgeType& edgeType,
                                  const ByteGraph::Neighbor& neighborNodes) = 0;

    virtual void DelFullGraphInfo() = 0;

    virtual void DelNodeFeature(const ByteGraph::NodeId& nodeId) = 0;

    virtual void DelNodeNeighbors(const ByteGraph::NodeId& nodeId, const ByteGraph::EdgeType& edgeType) = 0;

    // return a an estimate of the combined charges
    // of all elements stored in the cache.
    virtual size_t GetTotalCharge() const {
        // -1 means current not supported
        return -1;
    }
};

}  // namespace ByteCamp

#endif  // BYTEGRAPH_CACHE_BASE_H
