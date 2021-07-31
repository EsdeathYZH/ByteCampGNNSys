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

    virtual bool GetFullGraphInfo(ByteGraph::GraphInfo& graphInfo) = 0;

    virtual bool GetNodeFeature(const ByteGraph::NodeId& nodeId, ByteGraph::NodeFeature& nodeFeature) = 0;

    virtual bool GetNeighbors(const ByteGraph::NodeId& nodeId, const ByteGraph::EdgeType& edgeType,
                              ByteGraph::Neighbor& neighborNodes) = 0;

    virtual void PutFullGraphInfo(const ByteGraph::GraphInfo& graphInfo) = 0;

    virtual void PutNodeFeature(const ByteGraph::NodeId& nodeId, const ByteGraph::NodeFeature& nodeFeature) = 0;

    virtual void PutNodeNeighbors(const ByteGraph::NodeId& nodeId, const ByteGraph::Neighbor& neighborNodes) = 0;

    virtual bool DelFullGraphInfo() = 0;

    virtual bool DelNodeFeature(const ByteGraph::NodeId& nodeId) = 0;

    virtual bool DelNodeNeighbors(const ByteGraph::NodeId& nodeId) = 0;

    // return a an estimate of the combined charges
    // of all elements stored in the cache.
    virtual size_t GetTotalCharge() const {
        // -1 means current not supported
        return -1;
    }
};

}  // namespace ByteCamp

#endif  // BYTEGRAPH_CACHE_BASE_H
