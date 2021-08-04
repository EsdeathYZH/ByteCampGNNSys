#include "client_with_cache.h"

#include <glog/logging.h>

#include "cache_base.h"
#include "rpc_client.h"
#include "utils/utils.h"

using std::string;

using namespace ::ByteGraph;
using namespace ::ByteCamp;

ClientWithCache::ClientWithCache(const std::vector<std::pair<std::string, int>> &serverAddresses,
                                 std::shared_ptr<Cache> cache)
    : cache_(std::move(cache)) {
    rpc_clients_.reserve(serverAddresses.size());
    for (const auto &server : serverAddresses) {
        auto rpc_client = std::make_shared<RpcClient>(server.first, server.second);
        GraphInfo graphInfo;
        rpc_client->GetFullGraphInfo(graphInfo);
        server_weights_.emplace_back(graphInfo.total_weights_);
        rpc_clients_.emplace_back(rpc_client);
    }
    assert(rpc_clients_.size() == serverAddresses.size());
    CacheWarmUp();
}

void ClientWithCache::CacheWarmUp() {
    LOG(INFO) << "Cache Warmup start";
    const size_t up = 3e7, batch = 102400;
    size_t cnt = 1e7;
    while (cnt < up) {
        std::vector<NodeId> nodes(batch);
        for (NodeId i = cnt; i < cnt+batch; ++i) {
            nodes[i-cnt] = i;
        }
        const auto size = nodes.size();
        const auto rpc_clients_size = rpc_clients_.size();
        std::vector<std::vector<NodeId>> rpc_clients_nodes(3);
        for (const auto &notInCacheNode : nodes) {
            rpc_clients_nodes[notInCacheNode % 3].push_back(notInCacheNode);
        }
        for (size_t i = 0; i < rpc_clients_size; ++i) {
            auto tmpSize = rpc_clients_nodes[i].size();
            NodesFeature notInCacheNodesFeature(tmpSize);
//            rpc_clients_[i]->GetBatchNodeFeature(rpc_clients_nodes[i], 7, notInCacheNodesFeature);
//            assert(tmpSize == notInCacheNodesFeature.size());
            for (size_t j = 0; j < tmpSize; ++j) {
                // put node feature in cache
                cache_->PutNodeFeature(rpc_clients_nodes[i][j], notInCacheNodesFeature[j]);
            }
        }
        cnt += batch;
    }
    LOG(INFO) << "client cache warmup end";
}

void ClientWithCache::GetFullGraphInfo(ByteGraph::GraphInfo &graphInfo) {
    auto graphInfoPtr = cache_->GetFullGraphInfo();
    if (nullptr != graphInfoPtr) {
        graphInfo = *graphInfoPtr;
        return;
    }
    rpc_clients_[0]->GetFullGraphInfo(graphInfo);
    cache_->PutFullGraphInfo(graphInfo);
}

void ClientWithCache::SampleBatchNodes(const ByteGraph::NodeType &type, const int32_t &batchSize,
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

void ClientWithCache::GetNodeFeature(const std::vector<ByteGraph::NodeId> &nodes,
                                     const ByteGraph::FeatureType &featureType, ByteGraph::NodesFeature &nodesFeature) {
    int cnt = 0;
    auto nodesFeaturePtr = cache_->GetNodeFeature(nodes);
    const auto size = nodes.size();
    std::vector<NodeId> notInCacheNodes;
    // check whether node feature is in cache
    for (size_t i = 0; i < size; ++i) {
        if (nodesFeaturePtr[i] == nullptr) {
            notInCacheNodes.push_back(nodes[i]);
        } else {
            ++cnt;
        }
    }
    LOG(INFO) << "cache hit:" << cnt << " times in " << nodes.size() << "total times";
    const auto rpc_clients_size = rpc_clients_.size();
    std::vector<std::vector<NodeId>> rpc_clients_nodes(rpc_clients_size);
    for (const auto &notInCacheNode : notInCacheNodes) {
        rpc_clients_nodes[notInCacheNode % rpc_clients_size].push_back(notInCacheNode);
    }
    for (size_t i = 0; i < rpc_clients_size; ++i) {
        NodesFeature notInCacheNodesFeature;
        rpc_clients_[i]->GetBatchNodeFeature(rpc_clients_nodes[i], featureType, notInCacheNodesFeature);
        auto tmpSize = rpc_clients_nodes[i].size();
        assert(tmpSize == notInCacheNodesFeature.size());
        for (size_t j = 0; j < tmpSize; ++j) {
            // put node feature in cache
            cache_->PutNodeFeature(rpc_clients_nodes[i][j], notInCacheNodesFeature[j]);
        }
    }
    nodesFeaturePtr = cache_->GetNodeFeature(nodes);
    nodesFeature.clear();
    nodesFeature.reserve(size);
    for (size_t i = 0; i < size; ++i) {
        nodesFeature.push_back(*nodesFeaturePtr[i]);
    }
}

void ClientWithCache::GetNeighborsWithFeature(const ByteGraph::NodeId &nodeId, const ByteGraph::EdgeType &edgeType,
                                              const ByteGraph::FeatureType &featureType,
                                              std::vector<ByteGraph::IDFeaturePair> &neighbors) {
    ByteGraph::Neighbor neighborNodes;
    auto neighborNodesPtr = cache_->GetNeighbors(nodeId, edgeType);
    if (nullptr == neighborNodesPtr) {
        // degrade to rpc call
        rpc_clients_[nodeId % (rpc_clients_.size())]->GetNodeNeighbors(nodeId, edgeType, neighborNodes);
        cache_->PutNodeNeighbors(nodeId, edgeType, neighborNodes);
    }
    auto size = neighborNodes.size();
    // preallocate space
    neighbors.clear();
    neighbors.reserve(size);
    NodesFeature neighborNodesFeature;
    GetNodeFeature(neighborNodes, featureType, neighborNodesFeature);
    for (size_t i = 0; i < size; ++i) {
        IDFeaturePair tmp;
        tmp.node_id = neighborNodes[i];
        tmp.features.push_back(std::move(neighborNodesFeature[i]));
        neighbors.push_back(std::move(tmp));
    }
}

void ClientWithCache::SampleNeighbor(const int32_t &batchSize, const ByteGraph::NodeType &nodeType,
                                     const ByteGraph::EdgeType &edgeType, const int32_t &sampleNum,
                                     const ByteGraph::SampleStrategy::type &sampleStrategy,
                                     std::vector<std::vector<ByteGraph::NodeId>> &neighbors) {
    // todo(liuwenjing)
    //    rpc_client_->SampleNeighbor(batchSize, nodeType, neighborType, sampleNum, neighbors);
}

void ClientWithCache::RandomWalk(const int32_t& batchSize, const int32_t& walkLen,
                                 const ByteGraph::SampleStrategy::type &sampleStrategy,
                                 std::vector<std::vector<ByteGraph::NodeId>>& nodes) {
    //    rpc_client_->RandomWalk(batchSize, walkLen, nodes);
}
