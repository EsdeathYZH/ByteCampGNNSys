#include <fstream>
#include "graph/graph.h"

namespace Byte {
Graph::Graph(int partition_id, std::string data_dir) 
    : partition_id(partition_id) {
    load(data_dir);
}

NodeList Graph::getNeighbors(NodeID node_id, EdgeType edge_type) {
    NeighborList neigh_list = edge_map_[edge_type][node_id];
    NodeList node_list;
    node_list.data = &edge_data_[edge_type][neigh_list.index];
    node_list.sz = neigh_list.sz; 
    return node_list;
}

NodeList Graph::getNodes(NodeType node_type) {
    NodeList node_list;
    node_list.data = node_ids[node_type].data();
    node_list.sz = node_ids[node_type].size();
    return node_list;
}

Feature Graph::getFeatureData(NodeID node_id, FeatureType feat_type) {
    Feature result;
    FeatureTypeMeta meta = this->getFeatureTypeMeta(feat_type);
    uint32_t feat_dim = meta.feature_dim;
    uint32_t feat_num = feature_data_[feat_type].size() / feat_dim;
    uint32_t index = feature_map_[feat_type][node_id];
    if(feat_store_type == FeatureStorageType::COL_STORE) {
        result.data = feature_data_[feat_type].data() + index;
        result.sz = feat_dim;
        result.stride = feat_num;
    } else {
        result.data = feature_data_[feat_type].data() + index * feat_dim;
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
        result.data = feature_data_[feat_type].data() + feat_num * idx;
        result.sz = feat_num;
        result.stride = 1;
    } else {
        result.data = feature_data_[feat_type].data() + idx;
        result.sz = feat_num;
        result.stride = feat_dim;
    }
    return result;
}

void Graph::load(std::string data_dir) {
    // init metadata

    // load data
    std::string path = data_dir + "/" + std::to_string(partition_id);
    load_paper2paper_edges(path);
    load_author2paper_edges(path);
    load_author2institution_edges(path);
    load_paper_label(path + "/paper_label.txt");
    load_paper_feature(path + "/paper_feature.txt");
}

void Graph::load_paper2paper_edges(std::string file_path) {
    load_edges(4, file_path + "/paper2paper_edge_table.txt");
}

void Graph::load_author2paper_edges(std::string file_path) {
    load_edges(5, file_path + "/author2paper_edge_table.txt");
}

void Graph::load_author2institution_edges(std::string file_path) {
    load_edges(6, file_path + "/author2institution_edge_table.txt");
}

void Graph::load_edges(EdgeType edge_type, std::string file_path) {
    EdgeTypeMeta edge_meta = edge_type_meta_[edge_type];
    NodeTypeMeta src_meta = node_type_meta_[edge_meta.src_type];
    edge_map_[edge_type] = std::unordered_map<NodeID, NeighborList>();
    edge_map_[edge_type].reserve(src_meta.local_num);
    edge_data_[edge_type] = std::vector<NodeID>();
    edge_data_[edge_type].reserve(edge_meta.local_num);

    std::ifstream edge_file(file_path);
    NodeID src_id, dst_id, cur_id = 0;
    size_t prev_idx = 0, cur_idx = 0;
    // we assume edges have been sorted by source id
    while(edge_file >> src_id >> dst_id) {
        // new src
        if(src_id != cur_id) {
            NeighborList neigh_list;
            neigh_list.index = prev_idx;
            neigh_list.sz = cur_idx - prev_idx;
            edge_map_[edge_type][cur_id] = neigh_list;
            prev_idx = cur_idx;
            cur_id = src_id;
        }
        edge_data_[edge_type].push_back(dst_id);
        cur_idx++;
    }
    return;
}

void Graph::load_paper_feature(std::string file_path) {

}

void Graph::load_paper_label(std::string file_path) {
    label_map_.reserve(1398159); // FIXME: remove hardcode
    std::ifstream label_file(file_path);
    NodeID paper_id;
    uint32_t paper_label;
    while(label_file >> paper_id >> paper_label) {
        label_map_[paper_id] = paper_label;
    }
    return;
}

}