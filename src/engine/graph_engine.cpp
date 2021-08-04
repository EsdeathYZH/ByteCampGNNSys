#include "engine/graph_engine.h"

namespace Byte {

GraphEngine::GraphEngine(std::shared_ptr<Graph> graph, bool use_alias) 
    : graph_(graph), use_alias_(use_alias) {
    WeightList weights = graph_->getNodeWeights(PAPER, PAPER_FEATURE, 0); // FIXME: hard code
    its_table_ = std::make_shared<ITSSampling>(weights);
    alias_table_ = std::make_shared<ALIASSampling>(weights);
}

GraphMeta GraphEngine::getGraphInfo() {
    return graph_->getGraphMeta();
}

std::vector<NodeID> GraphEngine::sampleNodesFromRandom(NodeType node_type, int batch_size) {
    std::vector<NodeID> mini_batch;
    mini_batch.resize(batch_size);
    NodeList nodes = graph_->getNodes(node_type);
    for(int i = 0; i < batch_size; i++) {
        uint32_t idx = uint32_t(FastRandom::xorshf01() * nodes.sz);
        mini_batch[i] = nodes.data[idx];
    }
    return mini_batch;
}

std::vector<NodeID> GraphEngine::sampleNodesFromWeight(NodeType node_type, int batch_size, int feat_idx) {
    if(node_type != PAPER) {
        std::cout << "Only support paper weighted sampling" << std::endl;
    }
    std::vector<NodeID> mini_batch;
    mini_batch.reserve(batch_size);
    NodeList nodes = graph_->getNodes(node_type);
    // WeightList weights = graph_->getNodeWeights(node_type, PAPER_FEATURE, feat_idx); // FIXME: hard code
    // ITSSampling its_table(weights);
    for(int i = 0; i < batch_size; i++) {
        if(use_alias_) {
            mini_batch.push_back(nodes.data[alias_table_->sample()]);
        } else {
            mini_batch.push_back(nodes.data[its_table_->sample()]);
        }
    }
    return mini_batch;
}

Feature GraphEngine::getNodeFeature(NodeID node_id, FeatureType feat_type) {
    return graph_->getFeatureData(node_id, feat_type);
}

FeatureData GraphEngine::getNodeFeatureItem(NodeID node_id, FeatureType feat_type, int32_t feat_idx) {
    return graph_->getFeatureItem(node_id, feat_type, feat_idx);
}


NodeList GraphEngine::getNodeNeighbors(NodeID node_id, EdgeType edge_type) {
    return graph_->getNeighbors(node_id, edge_type);
}

std::vector<NodeID> GraphEngine::sampleNodeNeighborsFromRandom(NodeID node_id, EdgeType edge_type, int neigh_num) {
    std::vector<NodeID> results(neigh_num);
    NodeList neighbors = graph_->getNeighbors(node_id, edge_type);
    for(int i = 0; i < neigh_num; i++) {
        uint32_t idx = uint32_t(ThreadLocalRandom() * neighbors.sz);
        results[i] = neighbors.data[idx];
    }
    return results;
}

std::vector<NodeID> GraphEngine::randomWalk(NodeID root_node, EdgeType edge_type, int walk_len) {
    // FIXME: should be implemented by client?
    return std::vector<NodeID>();
}

}