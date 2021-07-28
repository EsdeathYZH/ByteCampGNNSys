#ifndef BYTEGRAPH_GRAPH_GRAPH_H_
#define BYTEGRAPH_GRAPH_GRAPH_H_

#include "graph/type.h"

namespace Byte {

class Graph {
    friend class DataSet;
public:
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

    // edge data
    std::unordered_map<EdgeType, std::unordered_map<NodeID, NeighborList>> edge_map_;
    std::unordered_map<EdgeType, std::vector<NodeID>> edge_data_;
};

}

#endif