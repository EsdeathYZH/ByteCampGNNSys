#ifndef BYTEGRAPH_GRAPH_GRAPH_ENGINE_H_
#define BYTEGRAPH_GRAPH_GRAPH_ENGINE_H_

#include <memory>
#include "graph/type.h"
#include "graph/graph.h"

namespace Byte {

enum class SamplingStrategy { 
    RANDOM,
    WEIGHT
};

class GraphEngine {
public:
    // used by 1st request type
    GraphMeta getGraphInfo();
    // used by 2nd & 3rd request type
    NodeList sampleNodesFromRandom(NodeType node_type, int batch_size);
    NodeList sampleNodesFromWeight(NodeType node_type, int batch_size);
    // used by 4th & 5th request type
    Feature getNodeFeature(NodeID node_id, FeatureType feat_type);
    // used by 5th request type
    NodeListPtr getNodeNeighbors(NodeID node_id, FeatureType feat_type);
    // used by 6th request type
    NodeList sampleNodeNeighbors(NodeID node_id, EdgeType edge_type, int neigh_num);
    // used by 7th request type
    NodeList randomWalk(NodeID root_node, EdgeType edge_type, int walk_len);
private:
    std::shared_ptr<Graph> graph_; 
};

}

#endif