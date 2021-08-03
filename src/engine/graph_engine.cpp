#include "engine/graph_engine.h"

namespace Byte {

GraphEngine::GraphEngine(std::shared_ptr<Graph> graph) : graph_(graph) {}

GraphMeta GraphEngine::getGraphInfo() {
    return graph_->getGraphMeta();
}

std::vector<NodeID> GraphEngine::sampleNodesFromRandom(NodeType node_type, int batch_size) {
    std::vector<NodeID> mini_batch;
    mini_batch.reserve(batch_size);
    NodeList nodes = graph_->getNodes(node_type);
    for(int i = 0; i < batch_size; i++) {
        uint32_t idx = uint32_t(ThreadLocalRandom() * nodes.sz);
        mini_batch.push_back(nodes.data[idx]);
    }
    return mini_batch;
}

std::vector<NodeID> GraphEngine::sampleNodesFromWeight(NodeType node_type, int batch_size, int feat_idx) {
    std::vector<NodeID> mini_batch;
    mini_batch.reserve(batch_size);
    NodeList nodes = graph_->getNodes(node_type);
    WeightList weights = graph_->getNodeWeights(node_type, PAPER_FEATURE, feat_idx); // FIXME: hard code
    ITSSampling its_table(weights);
    for(int i = 0; i < batch_size; i++) {
        mini_batch.push_back(its_table.sample());
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