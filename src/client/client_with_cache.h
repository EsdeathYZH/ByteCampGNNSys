#ifndef BYTEGRAPH_CLIENT_WITH_CACHE_H
#define BYTEGRAPH_CLIENT_WITH_CACHE_H

#include "client_base.h"
#include <vector>

namespace ByteCamp {

// forward declaration
class Cache;
class RpcClient;

class ClientWithCache : public ClientBase {
    ClientWithCache(const std::vector<std::pair<std::string, int>> &serverAddresses, std::shared_ptr<Cache> cache);

    void GetFullGraphInfo(ByteGraph::GraphInfo& graphInfo) override;

    void SampleBatchNodes(const ByteGraph::NodeType& type, const int32_t& batchSize,
                          const ByteGraph::SampleStrategy::type& sampleStrategy,
                          ByteGraph::BatchNodes& batchNodes) override;

    void GetNodeFeature(const std::vector<ByteGraph::NodeId>& nodes, const ByteGraph::FeatureType& featureId,
                        ByteGraph::NodesFeature& nodeFeature) override;

    void GetNeighborsWithFeature(const ByteGraph::NodeId& nodeId, const ByteGraph::EdgeType& neighborType,
                                 const ByteGraph::FeatureType& featureType,
                                 std::vector<ByteGraph::IDFeaturePair>& neighbors) override;

    void SampleNeighbor(const int32_t& batchSize, const ByteGraph::NodeType& nodeType,
                        const ByteGraph::NodeType& neighborType, const int32_t& sampleNum,
                        std::vector<ByteGraph::IDNeighborPair>& neighbors) override;

    void RandomWalk(const int32_t& batchSize, const int32_t& walkLen, std::vector<ByteGraph::NodeId>& nodes) override;

   private:
    std::vector<std::shared_ptr<RpcClient>> rpc_clients_;
    std::vector<double> server_weights_;
    std::shared_ptr<Cache> cache_;
};

}  // namespace ByteCamp

#endif  // BYTEGRAPH_CLIENT_WITH_CACHE_H
