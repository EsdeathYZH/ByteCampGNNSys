#ifndef BYTEGRAPH_GRAPH_GRAPH_H_
#define BYTEGRAPH_GRAPH_GRAPH_H_

#include <iostream>
#include <unordered_map>
#include <map>
#include <vector>
#include "graph/type.h"

namespace Byte {

class Graph {
public:
    explicit Graph(int partition_id, std::string data_dir);

    // meta data getter
    GraphMeta getGraphMeta() { return graph_meta_; }
    NodeTypeMeta getNodeTypeMeta(NodeType node_type) { return node_type_meta_[node_type]; }
    EdgeTypeMeta getEdgeTypeMeta(EdgeType edge_type) { return edge_type_meta_[edge_type]; }
    FeatureTypeMeta getFeatureTypeMeta(FeatureType feat_type) { return feature_type_meta_[feat_type]; }

    // data getter
    NodeList getNodes(NodeType node_type);
    NodeList getNeighbors(NodeID node_id, EdgeType edge_type);
    Feature getFeatureData(NodeID node_id, FeatureType feat_type);
    FeatureData getFeatureItem(NodeID node_id, FeatureType feat_type, uint32_t idx);
    WeightList getNodeWeights(NodeType node_type, FeatureType feat_type, uint32_t idx);

private:
    NodeType PAPER = 1;
    NodeType AUTHOR = 2;
    NodeType INSTITUTION = 3;
    EdgeType PAPER2PAPER = 4;
    EdgeType AUTHOR2PAPER = 5;
    EdgeType AUTHOR2INSTITUTION = 6;

    int partition_id;
    int paper_feat_dim = 768;
    int num_classes = 153;

    // meta data
    GraphMeta graph_meta_;
    std::map<NodeType, NodeTypeMeta> node_type_meta_;
    std::map<EdgeType, EdgeTypeMeta> edge_type_meta_;
    std::map<FeatureType, FeatureTypeMeta> feature_type_meta_;

    // node data
    std::unordered_map<NodeType, std::vector<NodeID>> node_ids;

    // feature data
    FeatureStorageType feat_store_type;
    std::unordered_map<FeatureType, std::unordered_map<NodeID, uint32_t>> feature_map_;
    std::unordered_map<FeatureType, std::vector<FeatureData>> feature_data_;

    // label data
    std::unordered_map<NodeID, FeatureData> label_map_;

    // edge data
    std::unordered_map<EdgeType, std::unordered_map<NodeID, NeighborList>> edge_map_;
    std::unordered_map<EdgeType, std::vector<NodeID>> edge_data_;

    // data loading
    void load(std::string data_dir);
    void load_edges(EdgeType edge_type, std::string file_path);
    void load_paper_nodes();
    void load_author_nodes();
    void load_institution_nodes();
    void load_paper2paper_edges(std::string file_path);
    void load_author2paper_edges(std::string file_path);
    void load_author2institution_edges(std::string file_path);

    void load_paper_feature(std::string file_path);
    void load_paper_label(std::string file_path);
};

}

#endif