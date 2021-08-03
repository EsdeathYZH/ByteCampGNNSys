#include "rpc_client.h"

#include <glog/logging.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TSocket.h>

using namespace ::ByteCamp;
using namespace ::ByteGraph;

using std::string;

namespace at = ::apache::thrift;
namespace atp = ::apache::thrift::protocol;
namespace att = ::apache::thrift::transport;

RpcClient::RpcClient(const std::string &peerIP, int port)
    : socket_(std::make_shared<att::TSocket>(peerIP, port))
    , transport_(std::static_pointer_cast<att::TTransport>(std::make_shared<att::TFramedTransport>(socket_)))
    , protocol_(std::static_pointer_cast<atp::TProtocol>(std::make_shared<atp::TBinaryProtocol>(transport_)))
    , rpc_client_(std::make_shared<ByteGraph::GraphServicesClient>(protocol_)) {
    // open connection, may throw exception.
    transport_->open();
    LOG(INFO) << "connect to server:" << peerIP << " port:" << port;
}

void RpcClient::GetFullGraphInfo(ByteGraph::GraphInfo &graphInfo) { rpc_client_->getFullGraphInfo(graphInfo); }

void RpcClient::SampleBatchNodes(const ByteGraph::NodeType &type, const int32_t &batchSize,
                                 const ByteGraph::SampleStrategy::type &sampleStrategy,
                                 ByteGraph::BatchNodes &batchNodes) {
    rpc_client_->SampleBatchNodes(batchNodes, type, batchSize, sampleStrategy);
}

void RpcClient::GetNodeFeature(const ByteGraph::NodeId &nodeId, const ByteGraph::FeatureType &featureType, ByteGraph::NodeFeature &nodeFeature){
    rpc_client_->GetNodeFeature(nodeFeature, nodeId, featureType);
}

void RpcClient::GetBatchNodeFeature(const std::vector<ByteGraph::NodeId> &nodes, const ByteGraph::FeatureType &featureType, ByteGraph::NodesFeature &nodesFeature) {
    rpc_client_->GetBatchNodeFeature(nodesFeature, nodes, featureType);
}

void RpcClient::GetNodeNeighbors(const ByteGraph::NodeId &nodeId, const ByteGraph::EdgeType &edgeType, ByteGraph::Neighbor &neighborNodes){
    rpc_client_->GetNodeNeighbors(neighborNodes, nodeId, edgeType);
}

void RpcClient::GetNeighborsWithFeature(const ByteGraph::NodeId &nodeId, const ByteGraph::EdgeType &neighborType,
                                        const ByteGraph::FeatureType &featureType,
                                        std::vector<ByteGraph::IDFeaturePair> &neighbors) {
    rpc_client_->GetNeighborsWithFeature(neighbors, nodeId, neighborType, featureType);
}

void RpcClient::SampleNeighbor(const int32_t &batchSize, const ByteGraph::NodeType &nodeType,
                               const ByteGraph::NodeType &neighborType, const int32_t &sampleNum,
                               std::vector<ByteGraph::IDNeighborPair> &neighbors) {
    // TODO(wenjing)
    //rpc_client_->SampleNodeNeighbors(neighbors, batchSize, nodeType, neighborType, sampleNum, ByteGraph::SampleStrategy::type::RANDOM);
}

void RpcClient::RandomWalk(const int32_t &batchSize, const int32_t &walkLen, std::vector<ByteGraph::NodeId> &nodes) {
    // TODO(zihang)
    //rpc_client_->RandomWalk(nodes, batchSize, walkLen);
}
