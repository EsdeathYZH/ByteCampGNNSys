#include "client_without_cache.h"

#include <glog/logging.h>

#include <unordered_map>

#include "rpc_client.h"
#include "utils/utils.h"

using std::string;

using namespace ::ByteGraph;
using namespace ::ByteCamp;

ClientWithoutCache::ClientWithoutCache(const std::vector<std::pair<std::string, int>> &serverAddresses) {
    rpc_clients_.reserve(serverAddresses.size());
    for (const auto &server : serverAddresses) {
        auto rpc_client = std::make_shared<RpcClient>(server.first, server.second);
        GraphInfo graphInfo;
        rpc_client->GetFullGraphInfo(graphInfo);
        server_weights_.emplace_back(graphInfo.total_weights_);
        rpc_clients_.emplace_back(rpc_client);
    }
    assert(rpc_clients_.size() == serverAddresses.size());
}

void ClientWithoutCache::GetFullGraphInfo(ByteGraph::GraphInfo &graphInfo) {
    rpc_clients_[0]->GetFullGraphInfo(graphInfo);
}

void ClientWithoutCache::SampleBatchNodes(const ByteGraph::NodeType &type, const int32_t &batchSize,
                                          const ByteGraph::SampleStrategy::type &sampleStrategy,
                                          const int32_t &featureIndex,
                                          ByteGraph::BatchNodes &batchNodes) {
    batchNodes.node_ids.clear();
    batchNodes.node_ids.reserve(batchSize);
    const auto size = rpc_clients_.size();
    std::vector<int64_t> servers_weight(size, 1);
    if (sampleStrategy != SampleStrategy::RANDOM) {
        for (size_t i = 0; i < size; ++i) {
            servers_weight[i] = server_weights_[i][featureIndex];
        }
    }
    auto rpc_clients_batch_node_size = GetBatchSizeAccordingToWeights(servers_weight, batchSize);
    for (size_t i = 0; i < size; ++i) {
        ByteGraph::BatchNodes tmpBatchNodes;
        rpc_clients_[i]->SampleBatchNodes(type, rpc_clients_batch_node_size[i], sampleStrategy, featureIndex, tmpBatchNodes);
        batchNodes.node_ids.insert(batchNodes.node_ids.end(), tmpBatchNodes.node_ids.begin(),
                                   tmpBatchNodes.node_ids.end());
    }
}

void ClientWithoutCache::GetNodeFeature(const std::vector<ByteGraph::NodeId> &nodes,
                                        const ByteGraph::FeatureType &featureType,
                                        ByteGraph::NodesFeature &nodeFeature) {
    nodeFeature.resize(nodes.size());
    const auto rpc_client_size = rpc_clients_.size();
    std::vector<std::vector<NodeId>> rpc_clients_nodes(rpc_client_size);
    size_t index = 0;
    std::unordered_map<NodeId, size_t> nodeId2Index;
    for (const auto &node : nodes) {
        // nodeId map to nodeFeature index
        nodeId2Index[node] = index++;
        rpc_clients_nodes[node % rpc_client_size].push_back(node);
    }
# ifdef DEBUG
    size_t sum = 0;
    for (const auto& rpc_clients_node : rpc_clients_nodes) {
        sum += rpc_clients_node.size();
    }
    assert(nodes.size() == sum);
#endif
    assert(index == nodes.size());
    for (size_t i = 0; i < rpc_client_size; ++i) {
        NodesFeature tmpNodesFeature;
        rpc_clients_[i]->GetBatchNodeFeature(rpc_clients_nodes[i], featureType, tmpNodesFeature);
        assert(tmpNodesFeature.size() == rpc_clients_nodes[i].size());
        for (size_t j = 0; j < tmpNodesFeature.size(); ++j) {
            nodeFeature[nodeId2Index[rpc_clients_nodes[i][j]]] = std::move(tmpNodesFeature[j]);
        }
    }
}

void ClientWithoutCache::GetNeighborsWithFeature(const ByteGraph::NodeId &nodeId,
                                                 const ByteGraph::EdgeType &neighborType,
                                                 const ByteGraph::FeatureType &featureType,
                                                 std::vector<ByteGraph::IDFeaturePair> &neighbors) {
    Neighbor neighborNodes;
    rpc_clients_[nodeId % (rpc_clients_.size())]->GetNodeNeighbors(nodeId, neighborType, neighborNodes);
    NodesFeature nodesFeature;
    GetNodeFeature(neighborNodes, featureType, nodesFeature);
    neighbors.clear();
    auto size = neighborNodes.size();
    neighbors.reserve(size);
    for (size_t i = 0; i < size; ++i) {
        IDFeaturePair idFeaturePair;
        idFeaturePair.node_id = neighborNodes[i];
        idFeaturePair.features.push_back(std::move(nodesFeature[i]));
        neighbors.push_back(std::move(idFeaturePair));
    }
}

void ClientWithoutCache::SampleNeighbor(const int32_t &batchSize, const ByteGraph::NodeType &nodeType,
                                        const ByteGraph::EdgeType &edgeType, const int32_t &sampleNum,
                                        const ByteGraph::SampleStrategy::type &sampleStrategy,
                                        std::vector<std::vector<ByteGraph::NodeId>> &neighbors) {
    // if(sampleStrategy == ByteGraph::SampleStrategy::type::RANDOM) {
    //     ByteGraph::BatchNodes nodes;
    //     SampleBatchNodes(nodeType, batchSize, ByteGraph::SampleStrategy::RANDOM, 0, nodes);
    //     rpc_clients_->SampleNodeNeighbors(nodes.node_ids, edgeType, sampleNum, neighbors);
    // } else if(sampleStrategy == ByteGraph::SampleStrategy::type::ITS) {
    //     ByteGraph::BatchNodes nodes;
    //     SampleBatchNodes(nodeType, batchSize, ByteGraph::SampleStrategy::RANDOM, 0, nodes);
    //     std::vector<ByteGraph::Neighbor> neighbors;
    //     rpc_clients_->GetBatchNodeNeighbors(nodes.node_ids, edgeType);
    //     std::unordered_set<ByteGraph::NodeId> neigh_set;
    //     for(auto neigh : neighbors) {
    //         for(auto id : neigh) neigh_set.insert(id);
    //     }
    //     std::vector<ByteGraph::NodeId> unique_neigh_ids(std::begin(neigh_set), std::end(neigh_set));
    //     rpc_client_->GetBatchNodeNeighbors(nodes.node_ids, edgeType);

    // }
}

void ClientWithoutCache::RandomWalk(const int32_t& batchSize, const int32_t& walkLen,
                            const ByteGraph::SampleStrategy::type &sampleStrategy,
                            std::vector<std::vector<ByteGraph::NodeId>>& nodes) {
    //    rpc_client_->RandomWalk(batchSize, walkLen, nodes);
}
