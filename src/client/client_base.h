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
                                  const int32_t& featureIndex,
                                  ByteGraph::BatchNodes& batchNodes) = 0;

    virtual void GetNodeFeature(const std::vector<ByteGraph::NodeId>& nodes, const ByteGraph::FeatureType& featureId,
                                ByteGraph::NodesFeature& nodeFeature) = 0;

    virtual void GetNeighborsWithFeature(const ByteGraph::NodeId& nodeId, const ByteGraph::EdgeType& neighborType,
                                         const ByteGraph::FeatureType& featureType,
                                         std::vector<ByteGraph::IDFeaturePair>& neighbors) = 0;

    virtual void SampleNeighbor(const int32_t &batchSize, const ByteGraph::NodeType &nodeType,
                                const ByteGraph::EdgeType &edgeType, const int32_t &sampleNum,
                                const ByteGraph::SampleStrategy::type &sampleStrategy,
                                std::vector<std::vector<ByteGraph::NodeId>> &neighbors) = 0;

    virtual void RandomWalk(const int32_t& batchSize, const int32_t& walkLen,
                            const ByteGraph::SampleStrategy::type &sampleStrategy,
                            std::vector<std::vector<ByteGraph::NodeId>>& nodes) = 0;
};

} // namespace ByteCamp

#endif // BYTEGRAPH_CLIENT_BASE_H_
