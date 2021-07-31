#ifndef BYTEGRAPH_CLIENT_BASE_H_
#define BYTEGRAPH_CLIENT_BASE_H_

#include <vector>

#include "gen-cpp/GraphServices.h"

namespace ByteCamp {

class ClientBase {
   public:
    virtual ~ClientBase() = default;

    virtual void GetFullGraphInfo(ByteGraph::GraphInfo& graphInfo) = 0;

    virtual void SampleBatchNodes(const ByteGraph::NodeType& type, const int32_t& batchSize,
                                  const ByteGraph::SampleStrategy::type& sampleStrategy,
                                  ByteGraph::BatchNodes& batchNodes) = 0;

    virtual void GetNodeFeature(const ByteGraph::NodeId& nodeId, const std::vector<ByteGraph::FeatureType>& featureId,
                                ByteGraph::NodeFeature& nodeFeature) = 0;

    virtual void GetNeighborsWithFeature(const ByteGraph::NodeId& nodeId, const ByteGraph::EdgeType& neighborType,
                                         const std::vector<ByteGraph::FeatureType>& featureTypes,
                                         std::vector<ByteGraph::IDFeaturePair>& neighbors) = 0;

    virtual void SampleNeighbor(const int32_t& batch_size, const ByteGraph::NodeType& nodeType,
                                const ByteGraph::NodeType& neighborType, const int32_t& sampleNum,
                                std::vector<ByteGraph::IDNeighborPair>& neighbors) = 0;

    virtual void RandomWalk(const int32_t& batchSize, const int32_t& walkLen,
                            std::vector<ByteGraph::NodeId>& nodes) = 0;
};

}  // namespace ByteCamp

#endif  // BYTEGRAPH_CLIENT_BASE_H_
