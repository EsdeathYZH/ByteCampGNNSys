#ifndef BYTEGRAPH_CACHE_BASE_H
#define BYTEGRAPH_CACHE_BASE_H

#include "gen-cpp/ByteGraph_types.h"

namespace ByteCamp {

class Cache {
   public:
    virtual ~Cache() = default;

    virtual bool GetFullGraphInfo(ByteGraph::GraphInfo &graphInfo) = 0;

    virtual bool GetNodeFeature(const ByteGraph::NodeId& nodeId, ByteGraph::NodeFeature& nodeFeature) = 0;

    virtual bool GetNeighbors(const ByteGraph::NodeId& nodeId, const ByteGraph::EdgeType& edgeType,
                              ByteGraph::Neighbor& neighborNodes) = 0;
};

}  // namespace ByteCamp

#endif  // BYTEGRAPH_CACHE_BASE_H
