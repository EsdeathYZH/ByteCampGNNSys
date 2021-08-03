#ifndef BYTEGRAPH_RPC_CLIENT_H
#define BYTEGRAPH_RPC_CLIENT_H

#include <memory>

#include "gen-cpp/GraphServices.h"

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

class RpcClient {
   public:
    RpcClient(const std::string& serverIP, int port);

    void GetFullGraphInfo(ByteGraph::GraphInfo& graphInfo);

    void SampleBatchNodes(const ByteGraph::NodeType& type, const int32_t& batchSize,
                          const ByteGraph::SampleStrategy::type& sampleStrategy,
                          ByteGraph::BatchNodes& batchNodes);

    void GetNodeFeature(const std::vector<ByteGraph::NodeId>& nodes, const ByteGraph::FeatureType& featureType,
                        ByteGraph::NodesFeature& nodeFeature);

    void GetNodeNeighbors(const ByteGraph::NodeId& nodeId, const ByteGraph::EdgeType& edgeType, ByteGraph::Neighbor& neighborNodes);

    void GetNeighborsWithFeature(const ByteGraph::NodeId& nodeId, const ByteGraph::EdgeType& neighborType,
                                 const ByteGraph::FeatureType& featureType,
                                 std::vector<ByteGraph::IDFeaturePair>& neighbors);

    void SampleNeighbor(const int32_t& batchSize, const ByteGraph::NodeType& nodeType,
                        const ByteGraph::NodeType& neighborType, const int32_t& sampleNum,
                        std::vector<ByteGraph::IDNeighborPair>& neighbors);

    void RandomWalk(const int32_t& batchSize, const int32_t& walkLen, std::vector<ByteGraph::NodeId>& nodes);
   private:
    std::shared_ptr<apache::thrift::transport::TSocket> socket_;
    std::shared_ptr<apache::thrift::transport::TTransport> transport_;
    std::shared_ptr<apache::thrift::protocol::TProtocol> protocol_;
    std::shared_ptr<ByteGraph::GraphServicesClient> rpc_client_;
};

}  // namespace ByteCamp

#endif  // BYTEGRAPH_RPC_CLIENT_H
