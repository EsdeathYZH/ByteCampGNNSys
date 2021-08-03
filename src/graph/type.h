#ifndef BYTEGRAPH_GRAPH_TYPE_H_
#define BYTEGRAPH_GRAPH_TYPE_H_

#include "stdint.h"
#include <vector>

namespace Byte {

using NodeID = int64_t;
using NodeType = uint32_t;
using EdgeType = uint32_t;
using FeatureType = uint32_t;
using FeatureData = float; // TODO: find a solution to use half-precision float (np.float16)

enum class FeatureStorageType { ROW_STORE, COL_STORE };

struct NodeList {
    NodeID* data;
    size_t sz;
};

struct Feature {
    FeatureData* data;
    size_t sz;
    size_t stride;
};

struct WeightList {
    float* data;
    size_t sz;
    size_t stride;
};

struct NeighborList
{
    uint64_t index; // offset in all neighbors
    size_t sz;
};

struct GraphMeta {
    uint64_t num_papers;
    uint64_t num_authors;
    uint64_t num_institutions;
    uint32_t paper_feat_dim;
    uint32_t num_classes;
    std::vector<float> sum_weights;
};

struct NodeTypeMeta {
    uint32_t local_num;
    uint32_t global_num;

    uint32_t dense_id_index;
};

struct EdgeTypeMeta {
    uint32_t local_num;
    uint32_t global_num;
    NodeType src_type;
    NodeType dst_type;
};

struct FeatureTypeMeta {
    uint32_t feature_dim;
    uint8_t item_bytes;
};

}

#endif