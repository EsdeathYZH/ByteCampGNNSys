#ifndef BYTEGRAPH_CLIENT_WITH_CACHE_H
#define BYTEGRAPH_CLIENT_WITH_CACHE_H

#include "client_base.h"

// forward declaration
namespace apache {

namespace thrift {

namespace protocol {

class TProtocol;

}  // namespace protocol

namespace transport {

class TSocket;
class TTransport;

}  // namespace transport

}  // namespace thrift
}  // namespace apache

namespace ByteCamp {

class Cache;

class ClientWithCache : public ClientBase {
    ClientWithCache(std::string peerIP, int port, std::shared_ptr<Cache> cache);

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
    std::shared_ptr<apache::thrift::transport::TSocket> socket_;
    std::shared_ptr<apache::thrift::transport::TTransport> transport_;
    std::shared_ptr<apache::thrift::protocol::TProtocol> protocol_;
    std::shared_ptr<ByteGraph::GraphServicesClient> rpc_client_;
    std::shared_ptr<Cache> cache_;
};

}  // namespace ByteCamp

#endif  // BYTEGRAPH_CLIENT_WITH_CACHE_H
