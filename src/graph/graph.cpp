#include <fstream>
#include <algorithm>
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
//    std::cout << "feat_num:" << feat_num << std::endl;
    uint32_t index = feature_map_[feat_type][node_id];
    if(feat_store_type == FeatureStorageType::COL_STORE) {
        result.data = feature_data_[feat_type].data() + index;
        result.sz = feat_dim;
        result.stride = feat_num;
    } else {
        result.data = feature_data_[feat_type].data() + index * feat_dim;
        result.sz = feat_dim;
//        std::cout << "feat_dim:" << feat_dim << std::endl;
        result.stride = 1;
    }
    return result;
}

FeatureData Graph::getFeatureItem(NodeID node_id, FeatureType feat_type, int32_t idx) {
    FeatureTypeMeta meta = this->getFeatureTypeMeta(feat_type);
    uint32_t feat_dim = meta.feature_dim;
    uint32_t index = feature_map_[feat_type][node_id];
    return feature_data_[feat_type][index * feat_dim + idx];
}

WeightList Graph::getNodeWeights(NodeType node_type, FeatureType feat_type, int32_t idx) {
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
    std::cout << "Begin to load graph data..." << std::endl;
    // init metadata
    graph_meta_.num_papers = 121751666;
    graph_meta_.num_authors = 122383112;
    graph_meta_.num_institutions = 25721;
    graph_meta_.paper_feat_dim = 768;
    graph_meta_.num_classes = 153;

    NodeTypeMeta paper_meta;
    NodeTypeMeta author_meta;
    NodeTypeMeta institution_meta;

    node_type_meta_[PAPER] = paper_meta;
    node_type_meta_[AUTHOR] = author_meta;
    node_type_meta_[INSTITUTION] = institution_meta;

    EdgeTypeMeta paper2paper_meta;
    paper2paper_meta.src_type = PAPER;
    paper2paper_meta.dst_type = PAPER;
    paper2paper_meta.global_num = 1297748926;
    EdgeTypeMeta author2paper_meta;
    author2paper_meta.src_type = AUTHOR;
    author2paper_meta.dst_type = PAPER;
    author2paper_meta.global_num = 386022720;
    EdgeTypeMeta author2institution_meta;
    author2institution_meta.src_type = AUTHOR;
    author2institution_meta.dst_type = INSTITUTION;
    author2institution_meta.global_num = 44592586;

    edge_type_meta_[PAPER2PAPER] = paper2paper_meta;
    edge_type_meta_[AUTHOR2PAPER] = author2paper_meta;
    edge_type_meta_[AUTHOR2INSTITUTION] = author2institution_meta;

    FeatureTypeMeta paper_feature_meta;
    paper_feature_meta.feature_dim = graph_meta_.paper_feat_dim;
    paper_feature_meta.item_bytes = 4;
    FeatureTypeMeta paper_label_meta;
    paper_label_meta.feature_dim = graph_meta_.num_classes;
    paper_label_meta.item_bytes = 4;

    feature_type_meta_[PAPER_FEATURE] = paper_feature_meta;
    feature_type_meta_[PAPER_LABEL] = paper_label_meta;

    // load data
    std::string path = data_dir + "/" + std::to_string(partition_id);
    load_paper_nodes();
    load_author_nodes();
    load_institution_nodes();
    load_paper2paper_edges(path);
    load_author2paper_edges(path);
    load_author2institution_edges(path);
    load_paper_label(path + "/paper_label.txt");
    load_paper_feature(path + "/paper_feature.txt");
}

void Graph::load_paper_nodes() {
    node_ids[PAPER] = std::vector<NodeID>();
    node_ids[PAPER].reserve(graph_meta_.num_papers / 3);
    NodeID start = 0;
    NodeID end = graph_meta_.num_papers;
    for(NodeID i = start; i < end; i++) {
        if(i % 3 == partition_id) node_ids[PAPER].push_back(i);
    }
    node_type_meta_[PAPER].local_num = node_ids[PAPER].size();
    std::cout << "paper local num:" << node_ids[PAPER].size() << std::endl;
}

void Graph::load_author_nodes() {
    node_ids[AUTHOR] = std::vector<NodeID>();
    node_ids[AUTHOR].reserve(graph_meta_.num_authors / 3);
    NodeID start = graph_meta_.num_papers;
    NodeID end = graph_meta_.num_papers + graph_meta_.num_authors;
    for(NodeID i = start; i < end; i++) {
        if(i % 3 == partition_id) node_ids[AUTHOR].push_back(i);
    }
    node_type_meta_[AUTHOR].local_num = node_ids[AUTHOR].size();
    std::cout << "author local num:" << node_ids[AUTHOR].size() << std::endl;
}

void Graph::load_institution_nodes() {
    node_ids[INSTITUTION] = std::vector<NodeID>();
    node_ids[INSTITUTION].reserve(graph_meta_.num_institutions / 3);
    NodeID start = graph_meta_.num_papers + graph_meta_.num_authors;
    NodeID end = graph_meta_.num_papers + graph_meta_.num_authors + graph_meta_.num_institutions;
    for(NodeID i = start; i < end; i++) {
        if(i % 3 == partition_id) node_ids[INSTITUTION].push_back(i);
    }
    node_type_meta_[INSTITUTION].local_num = node_ids[INSTITUTION].size();
    std::cout << "institution local num:" << node_ids[INSTITUTION].size() << std::endl;
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
    edge_data_[edge_type].reserve(edge_meta.global_num/3);

    std::ifstream edge_file(file_path);
    NodeID src_id, dst_id, cur_id = 0;
    size_t prev_idx = 0, cur_idx = 0;
    std::vector<std::pair<NodeID, int>> degree_vec;
    degree_vec.reserve(src_meta.local_num);
    // we assume edges have been sorted by source id
    while(edge_file >> src_id >> dst_id) {
        // new src
        if(src_id != cur_id) {
            NeighborList neigh_list;
            neigh_list.index = prev_idx;
            neigh_list.sz = cur_idx - prev_idx;
            degree_vec.push_back(std::pair<NodeID, int>(cur_id, neigh_list.sz));
            edge_map_[edge_type][cur_id] = neigh_list;
            prev_idx = cur_idx;
            cur_id = src_id;
        }
        edge_data_[edge_type].push_back(dst_id);
        cur_idx++;
        if(cur_idx % 10000000 == 0) {
            std::cout << "Processing " << cur_idx 
                      << " edges, edge type:" << edge_type << std::endl;
        }
    }

    // generate degree file
    // std::ofstream degree_file("/data/"+ std::to_string(partition_id)+"/paper2paper_degree.txt");
    // std::sort(std::begin(degree_vec), std::end(degree_vec),
    //         [](std::pair<NodeID, int>& left, std::pair<NodeID, int>& right){ 
    //             return left.second > right.second; 
    //         });
    // for(auto& pair : degree_vec) {
    //     degree_file << pair.first << " " << pair.second << "\n";
    // }
    // degree_file.close();

    edge_type_meta_[edge_type].local_num = cur_idx;
    std::cout << "edge type " << edge_type << " local num:" << cur_idx 
              << " src num:" << edge_map_[edge_type].size() << std::endl;
    return;
}

void Graph::load_paper_feature(std::string file_path) {
    // TODO: use real feature data
    uint64_t paper_num = node_ids[PAPER].size();
    FeatureTypeMeta feat_meta = feature_type_meta_[PAPER_FEATURE];
    feature_map_[PAPER_FEATURE] = std::unordered_map<NodeID, uint64_t>();
    feature_map_[PAPER_FEATURE].reserve(paper_num);
    feature_data_[PAPER_FEATURE] = std::vector<FeatureData>(paper_num * feat_meta.feature_dim);
    for(uint64_t i = 0; i < paper_num; i++) {
        feature_map_[PAPER_FEATURE][node_ids[PAPER][i]] = i;
    }
    #pragma omp parallel for num_threads(64)
    for(uint64_t i = 0; i < paper_num; i++) { // generate papers' feature
        for(uint64_t idx = i * feat_meta.feature_dim; idx < (i+1) * feat_meta.feature_dim; idx++) {
            feature_data_[PAPER_FEATURE][idx] = i % 2 + 0.14159;
        }
    }
    std::cout << "Generate feature data over." << std::endl;
    graph_meta_.sum_weights.resize(graph_meta_.paper_feat_dim);
    #pragma omp parallel for num_threads(64)
    for (uint64_t idx = 0; idx < graph_meta_.paper_feat_dim; idx++) {
        double sum_weight = 0;
        for(uint64_t i = 0; i < paper_num; i++) {
            sum_weight += feature_data_[PAPER_FEATURE][i*graph_meta_.paper_feat_dim+idx];
        }
        graph_meta_.sum_weights[idx] = sum_weight;
    }
    std::cout << "Calculate weight sum over." << std::endl;
}

void Graph::load_paper_label(std::string file_path) {
    label_map_.reserve(1398159); // FIXME: remove hardcode
    std::ifstream label_file(file_path);
    NodeID paper_id;
    FeatureData paper_label;
    while(label_file >> paper_id >> paper_label) {
        if(paper_id % 3 == partition_id) {
            label_map_[paper_id] = paper_label;
        }
    }
    std::cout << "label num:" <<  label_map_.size() << std::endl;
    return;
}

}