namespace cpp ByteGraph

typedef i64 NodeId
typedef i64 NodeType
typedef list<i32> NodeFeature
typedef i64 FeatureType
typedef list<NodeId> Neighbor
typedef i64 EdgeType
typedef list<NodeFeature> NodesFeature

// 获取全图信息
struct GraphInfo {
    1:list<i32> infos_;
    2:list<i32> total_weights_;
}

// 全局采样一个Batch的Paper类型点
enum SampleStrategy {
    ITS = 0,
    ALIAS = 1,
    RANDOM = 2
}

struct BatchNodes {
    1:list<NodeId> node_ids;
}

// 指定Author ID，拉取其P类型(边类型）的一跳邻居并返回特征（P为参数)
struct IDFeaturePair {
    1:NodeId node_id;
    2:list<NodeFeature> features;
}

// 指定点类型（类型为参数）......带权随机采样
struct IDNeighborPair {
    1:NodeId node_id;
    2:list<NodeId> neighbor_ids;
}

// 步长为N的随机游走
enum RandomWalkStrategy {
    BASIC_RANDOM = 0,
    KNGIHTKING = 1
}

service GraphServices {
    string  sayHello(1:i32 workerId, 2:string content)

    // 获取全图信息
    GraphInfo getFullGraphInfo()
    // 全局采样一个Batch的Paper类型点
    BatchNodes SampleBatchNodes(1:NodeType type, 2:i32 batch_size, 3:SampleStrategy strategy)
    // 获取指定点特征
    NodeFeature GetNodeFeature(1:NodeId node, 2:FeatureType feat_type)
    NodesFeature GetBatchNodeFeature(1:list<NodeId> nodes, 2:FeatureType feat_type)
    // 获取指定点邻居
    Neighbor GetNodeNeighbors(1:NodeId node, 2:EdgeType edge_type)
    list<Neighbor> GetBatchNodeNeighbors(1:list<NodeId> nodes, 2:EdgeType edge_type)
    // 指定Author ID，拉取其P类型(边类型）的一跳邻居并返回特征（P为参数)
    list<IDFeaturePair> GetNeighborsWithFeature(1:NodeId node_id, 2:EdgeType edge_type, 3:FeatureType feat_type)
    // 获取指定点权重
    list<i32> GetNodeWeights(1:list<NodeId> nodes, 2:FeatureType feat_type, 3:i32 feat_idx)
    // 指定点类型（类型为参数）......带权随机采样
    list<list<NodeId>> SampleNodeNeighbors(1:list<NodeId> nodes , 2:EdgeType edge_type, 3:i32 sample_num)
    // 步长为N的随机游走
    list<NodeId> RandomWalk(1:i32 batch_size, 2:i32 walk_len)

}
