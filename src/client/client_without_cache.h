#ifndef BYTEGRAPH_CLIENT_WITHOUT_CACHE_H
#define BYTEGRAPH_CLIENT_WITHOUT_CACHE_H

#include <memory>
#include <string>

#include "client_base.h"

namespace ByteCamp {

// forward declaration
class RpcClient;

class ClientWithoutCache : public ClientBase {
   public:
    ClientWithoutCache(std::string peerIP, int port);

    void GetFullGraphInfo(ByteGraph::GraphInfo& graphInfo) override;

    void SampleBatchNodes(const ByteGraph::NodeType& type, const int32_t& batchSize,
                          const ByteGraph::SampleStrategy::type& sampleStrategy,
                          ByteGraph::BatchNodes& batchNodes) override;

    void GetNodeFeature(const std::vector<ByteGraph::NodeId>& nodes, const ByteGraph::FeatureType& featureType,
                        ByteGraph::NodesFeature& nodeFeature) override;

    void GetNeighborsWithFeature(const ByteGraph::NodeId& nodeId, const ByteGraph::EdgeType& neighborType,
                                 const ByteGraph::FeatureType& featureType,
                                 std::vector<ByteGraph::IDFeaturePair>& neighbors) override;

    void SampleNeighbor(const int32_t& batchSize, const ByteGraph::NodeType& nodeType,
                        const ByteGraph::NodeType& neighborType, const int32_t& sampleNum,
                        std::vector<ByteGraph::IDNeighborPair>& neighbors) override;

    void RandomWalk(const int32_t& batchSize, const int32_t& walkLen, std::vector<ByteGraph::NodeId>& nodes) override;

   private:
    std::shared_ptr<RpcClient> rpc_client_;
};

}  // namespace ByteCamp

#endif  // BYTEGRAPH_CLIENT_WITHOUT_CACHE_H
