#ifndef BYTEGRAPH_CLIENT_WITHOUT_CACHE_H
#define BYTEGRAPH_CLIENT_WITHOUT_CACHE_H

#include <memory>
#include <string>
#include <unordered_set>

#include "client_base.h"

namespace ByteCamp {

// forward declaration
class RpcClient;

class ClientWithoutCache : public ClientBase {
   public:
    ClientWithoutCache(const std::vector<std::pair<std::string, int>> &serverAddresses);

    void GetFullGraphInfo(ByteGraph::GraphInfo& graphInfo) override;

    void SampleBatchNodes(const ByteGraph::NodeType& type, const int32_t& batchSize,
                          const ByteGraph::SampleStrategy::type& sampleStrategy,
                          const int32_t &featureIndex,
                          ByteGraph::BatchNodes& batchNodes) override;

    void GetNodeFeature(const std::vector<ByteGraph::NodeId>& nodes, const ByteGraph::FeatureType& featureType,
                        ByteGraph::NodesFeature& nodeFeature) override;

    void GetNeighborsWithFeature(const ByteGraph::NodeId& nodeId, const ByteGraph::EdgeType& neighborType,
                                 const ByteGraph::FeatureType& featureType,
                                 std::vector<ByteGraph::IDFeaturePair>& neighbors) override;

    void SampleNeighbor(const int32_t &batchSize, const ByteGraph::NodeType &nodeType,
                        const ByteGraph::EdgeType &edgeType, const int32_t &sampleNum,
                        const ByteGraph::SampleStrategy::type &sampleStrategy,
                        std::vector<std::vector<ByteGraph::NodeId>> &neighbors) override;

    void RandomWalk(const int32_t& batchSize, const int32_t& walkLen,
                    const ByteGraph::SampleStrategy::type &sampleStrategy,
                    std::vector<std::vector<ByteGraph::NodeId>>& nodes) override;

   private:
    std::vector<std::shared_ptr<RpcClient>> rpc_clients_;
    std::vector<std::vector<int64_t>> server_weights_;
};

}  // namespace ByteCamp

#endif  // BYTEGRAPH_CLIENT_WITHOUT_CACHE_H
