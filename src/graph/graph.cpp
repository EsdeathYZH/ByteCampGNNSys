#include "graph/graph.h"

namespace Byte {

NodeList Graph::getNeighbors(NodeID node_id, EdgeType edge_type) {
    NeighborList neigh_list = edge_map_[edge_type][node_id];
    return NodeList(&edge_data_[edge_type][neigh_list.index], neigh_list.sz);
}

NodeList Graph::getNodes(NodeType node_type) {
    return NodeList(node_ids[node_type].data(), node_ids[node_type].size());
}

Feature Graph::getFeatureData(NodeID node_id, FeatureType feat_type) {
    Feature result;
    FeatureTypeMeta meta = this->getFeatureTypeMeta(feat_type);
    uint32_t feat_dim = meta.feature_dim;
    uint32_t feat_num = feature_data_[feat_type].size() / feat_dim;
    uint32_t index = feature_map_[feat_type][node_id];
    if(feat_store_type == FeatureStorageType::COL_STORE) {
        result.data = feature_data_[feat_type][index];
        result.sz = feat_dim;
        result.stride = feat_num;
    } else {
        result.data = feature_data_[feat_type][index * feat_dim];
        result.sz = feat_dim;
        result.stride = 1;
    }
    return result;
}

FeatureData Graph::getFeatureItem(NodeID node_id, FeatureType feat_type, uint32_t idx) {
    FeatureTypeMeta meta = this->getFeatureTypeMeta(feat_type);
    uint32_t feat_dim = meta.feature_dim;
    uint32_t index = feature_map_[feat_type][node_id];
    return feature_data_[feat_type][index * feat_dim + idx];
}

WeightList Graph::getNodeWeights(NodeType node_type, FeatureType feat_type, uint32_t idx) {
    WeightList result;
    FeatureTypeMeta meta = this->getFeatureTypeMeta(feat_type);
    uint32_t feat_dim = meta.feature_dim;
    uint32_t feat_num = feature_data_[feat_type].size() / feat_dim;
    if(feat_store_type == FeatureStorageType::COL_STORE) {
        result.data = feature_data_[feat_type][feat_num * idx];
        result.sz = feat_num;
        result.stride = 1;
    } else {
        result.data = feature_data_[feat_type][idx];
        result.sz = feat_num;
        result.stride = feat_dim;
    }
    return result;
}

}