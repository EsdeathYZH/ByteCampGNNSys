#ifndef BYTEGRAPH_GRAPH_GRAPH_ENGINE_H_
#define BYTEGRAPH_GRAPH_GRAPH_ENGINE_H_

#include <memory>
#include "graph/type.h"
#include "graph/graph.h"
#include "engine/its_sampling.h"
#include "engine/alias_sampling.h"

namespace Byte {

enum class SamplingStrategy { 
    RANDOM,
    WEIGHT
};

class GraphEngine {
public:
    explicit GraphEngine(std::shared_ptr<Graph> graph);
    // used by 1st request type
    GraphMeta getGraphInfo();
    // used by 2nd & 3rd request type
    std::vector<NodeID> sampleNodesFromRandom(NodeType node_type, int batch_size);
    std::vector<NodeID> sampleNodesFromWeight(NodeType node_type, int batch_size, int feat_idx);
    // used by 4th & 5th request type
    Feature getNodeFeature(NodeID node_id, FeatureType feat_type);
    // used by 5th request type
    FeatureData getNodeFeatureItem(NodeID node_id, FeatureType feat_type, int32_t feat_idx);
    NodeList getNodeNeighbors(NodeID node_id, EdgeType edge_type);
    // used by 6th request type
    std::vector<NodeID> sampleNodeNeighborsFromRandom(NodeID node_id, EdgeType edge_type, int neigh_num);
    // used by 7th request type
    std::vector<NodeID> randomWalk(NodeID root_node, EdgeType edge_type, int walk_len);
private:
    std::shared_ptr<Graph> graph_; 
};

}

#endif