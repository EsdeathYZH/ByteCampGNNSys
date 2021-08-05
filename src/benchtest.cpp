#include <glog/logging.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#include <algorithm>
#include <thread>

#include "client/client_without_cache.h"
#include "client/client_with_cache.h"
#include "../src/utils/utils.h"

#include "client/graph_aware_cache.h"
#include "client/lru_cache.h"
#include "client/simple_cache.h"

using std::string;
using std::thread;
using std::vector;
using namespace ::ByteGraph;
using namespace ByteCamp;

const int32_t BATCH_SIZE = 1024;
const int32_t NODE_TYPE = 1;
const int32_t FEATURE_TYPE = 7;
const int32_t NEIGHBOR_TYPE = 2;
const int32_t NODE_ID = 1;

std::vector<NodeId> node_id_list;
std::vector<NodeId> edge_type_list;
std::vector<NodeId> feature_type_list;

int64_t get_wall_time() {
    struct timeval time;
    if (gettimeofday(&time, NULL)) {
        //  Handle error
        return 0;
    }
    return time.tv_sec * 1000000 + time.tv_usec;
}

void test_SampleBatchNodes(const std::vector<std::pair<string, int>>& servers, const NodeId& NODE_ID_MAX,
                           const EdgeType& NEIGHBOR_TYPE_MAX, const FeatureType& FEATURE_TYPE_MAX) {
    auto client = std::make_shared<ClientWithoutCache>(servers);

    BatchNodes batchNodes;
    const int32_t featureIndex = 3;

    for (NodeId node_id = 0; node_id < NODE_ID_MAX; node_id++) {
        for (EdgeType edge_type = 0; edge_type < NEIGHBOR_TYPE_MAX; edge_type++) {
            for (FeatureType feature_type = 0; feature_type < FEATURE_TYPE_MAX; feature_type++) {
                client->SampleBatchNodes(NODE_TYPE, BATCH_SIZE, SampleStrategy::RANDOM, featureIndex, batchNodes);
            }
        }
    }
}

void test_SampleBatchNodesAndGetNodeFeature(const std::vector<std::pair<string, int>>& servers,
                                            const std::shared_ptr<ByteCamp::Cache>& cache, const NodeId& NODE_ID_MAX,
                                            const EdgeType& NEIGHBOR_TYPE_MAX, const FeatureType& FEATURE_TYPE_MAX) {
    auto client = std::make_shared<ClientWithCache>(servers, cache);

    BatchNodes batchNodes;
    const int32_t featureIndex = 3;

    for (NodeId node_id = 0; node_id < NODE_ID_MAX; node_id++) {
        for (EdgeType edge_type = 0; edge_type < NEIGHBOR_TYPE_MAX; edge_type++) {
            for (FeatureType feature_type = 0; feature_type < FEATURE_TYPE_MAX; feature_type++) {
                client->SampleBatchNodes(NODE_TYPE, BATCH_SIZE, SampleStrategy::RANDOM, featureIndex, batchNodes);
                std::vector<NodeFeature> nodeFeatures;
                client->GetNodeFeature(batchNodes.node_ids, 7, nodeFeatures);
            }
        }
    }
}

// void test_GetNeighborsWithFeature(const std::vector<std::pair<string, int>>& servers, const NodeId& NODE_ID_MAX,
//                                   const EdgeType& NEIGHBOR_TYPE_MAX, const FeatureType& FEATURE_TYPE_MAX) {
//     auto client = std::make_shared<ClientWithoutCache>(servers);

//     std::vector<IDFeaturePair> neighbors;

//     for (NodeId node_id = 0; node_id < NODE_ID_MAX; node_id++) {
//         for (EdgeType edge_type = 0; edge_type < NEIGHBOR_TYPE_MAX; edge_type++) {
//             for (FeatureType feature_type = 0; feature_type < FEATURE_TYPE_MAX; feature_type++) {
//                 client->GetNeighborsWithFeature(node_id, edge_type, feature_type, neighbors);
//             }
//         }
//     }
// }

void test_GetNeighborsWithFeature(const std::vector<std::pair<string, int>>& servers, const NodeId& NODE_ID_MAX,
                                  const EdgeType& NEIGHBOR_TYPE_MAX, const FeatureType& FEATURE_TYPE_MAX) {
    // auto client = std::make_shared<ClientWithoutCache>(servers);
    std::shared_ptr<ByteCamp::Cache> simpleCache;
    std::unordered_map<ByteGraph::NodeId, int32_t> inDegree;
    if (NEIGHBOR_TYPE_MAX == 1) {
        simpleCache = NewSimpleCache(100 * (1ll << 30));
        LOG(INFO) << "NewSimpleCache over!";
    } else if (NEIGHBOR_TYPE_MAX == 2) {
        simpleCache = NewLRUCache(100 * (1ll << 30));
        LOG(INFO) << "NewLRUCache over!";
    } else if (NEIGHBOR_TYPE_MAX == 3) {
        inDegree = ReadInDegree("/data/sortedByIndegree_vertex_table.txt");
        LOG(INFO) << "ReadInDegree over!";
        simpleCache = NewGraphAwareCache(100 * (1ll << 30),
                                         [&inDegree](NodeId x, NodeId y) -> bool { return inDegree[x] > inDegree[y]; });
    }

    auto client = std::make_shared<ClientWithCache>(servers, simpleCache);

    std::vector<IDFeaturePair> neighbors;
    BatchNodes batchNodes;
    client->SampleBatchNodes(NODE_TYPE, NODE_ID_MAX, SampleStrategy::RANDOM, 0, batchNodes);
    assert(batchNodes.node_ids.size() == NODE_ID_MAX);

    int64_t start_t = get_wall_time();

    for (FeatureType feature_type = 0; feature_type < FEATURE_TYPE_MAX; feature_type++) {
        for (EdgeType edge_type = 0; edge_type < 1; edge_type++) {
            for (size_t index = 0; index < NODE_ID_MAX; index++) {
                client->GetNeighborsWithFeature(batchNodes.node_ids[index], 4, 7, neighbors);
            }
        }
    }

    int64_t end_t = get_wall_time();

    printf("type %lld: pure %lld times cost = %f ms, qps = %f\n", NEIGHBOR_TYPE_MAX, NODE_ID_MAX * FEATURE_TYPE_MAX,
            (end_t - start_t) / 1000.0, NODE_ID_MAX * FEATURE_TYPE_MAX * 1000000.0 / (end_t - start_t));
}

int main(int argc, char** argv) {
    // Initialize Google’s logging library.
    google::InitGoogleLogging(argv[0]);
    // google::SetLogDestination(google::INFO, "/tmp/log/INFO_");
    // google::SetStderrLogging(google::WARNING);
    // FLAGS_logtostderr = true;

    if (argc < 4) {
        printf("Usage: benchtest <server_list> <thread_nums> <retry_times> [optional: <node_id> <neighbor_type> <feature_type>]\n");
        return 0;
    }

    const char* server_list_file = argv[1];
    const int32_t thr_num = atoi(argv[2]);
    int64_t retry_times = atoll(argv[3]) * thr_num;
    NodeId NODE_ID_MAX = (NodeId) atoll(argv[3]);

    EdgeType NEIGHBOR_TYPE_MAX = 1;
    FeatureType FEATURE_TYPE_MAX = 1;
    if (argc == 6) {
        NEIGHBOR_TYPE_MAX = (EdgeType) atoll(argv[4]);
        FEATURE_TYPE_MAX = (FeatureType) atoll(argv[5]);
        retry_times = NODE_ID_MAX * NEIGHBOR_TYPE_MAX * FEATURE_TYPE_MAX * thr_num;
    }

    std::vector<std::pair<string, int>> servers = ReadConfig<string, int>(server_list_file);
    for (auto e : servers) {
        printf("%s %d\n", e.first.c_str(), e.second);
    }
    // auto client = std::make_shared<ClientWithoutCache>(servers);

    FeatureType featureType = 0;
    std::vector<IDFeaturePair> neighbors;
    LOG(INFO) << "GetNeighborsWithFeature";

    // NodeId shuffle
    for (NodeId node_id = 0; node_id < NODE_ID_MAX; node_id++) {
        node_id_list.emplace_back(node_id);
    }
    std::random_shuffle(node_id_list.begin(), node_id_list.end());

    // EdgeType shuffle
    for (EdgeType edge_type = 0; edge_type < NEIGHBOR_TYPE_MAX; edge_type++) {
        edge_type_list.emplace_back(edge_type);
    }
    std::random_shuffle(edge_type_list.begin(), edge_type_list.end());

    // FeatureType shuffle
    for (FeatureType feature_type = 0; feature_type < FEATURE_TYPE_MAX; feature_type++) {
        feature_type_list.emplace_back(feature_type);
    }
    std::random_shuffle(feature_type_list.begin(), feature_type_list.end());

    vector<thread> thrs;
    thrs.reserve(thr_num);

    int64_t start_t = get_wall_time();
    // printf("benchtest start\n");

    for (int i = 0; i < thr_num; ++i) {
        thrs.emplace_back(
            // thread(bind(test_GetNeighborsWithFeature, servers, NODE_ID_MAX, NEIGHBOR_TYPE_MAX, FEATURE_TYPE_MAX)));
            // thread(bind(test_SampleBatchNodesAndGetNodeFeature, servers, graphAwareCache, NODE_ID_MAX, NEIGHBOR_TYPE_MAX, FEATURE_TYPE_MAX)));
            thread(bind(test_GetNeighborsWithFeature, servers, NODE_ID_MAX, NEIGHBOR_TYPE_MAX, FEATURE_TYPE_MAX)));
    }
    for (auto& th : thrs) {
        th.join();
    }

    // printf("benchtest end\n");
    int64_t end_t = get_wall_time();

    // fprintf(stderr, "test_SampleBatchNodesAndGetNodeFeature %lld times cost = %f ms, qps = %f\n", retry_times,
    //         (end_t - start_t) / 1000.0, retry_times * 1000000.0 / (end_t - start_t));

    // start_t = get_wall_time();
    // printf("benchtest start\n");

    // for (int i = 0; i < thr_num; ++i) {
    //     thrs.emplace_back(
    //         thread(bind(test_SampleBatchNodes, servers, NODE_ID_MAX, NEIGHBOR_TYPE_MAX, FEATURE_TYPE_MAX)));
    // }
    // for (auto& th : thrs) {
    //     th.join();
    // }

    // printf("benchtest end\n");
    // end_t = get_wall_time();

    // fprintf(stderr, "test_SampleBatchNodes %lld times cost = %f ms, qps = %f\n", retry_times, (end_t - start_t) /
    // 1000.0,
    //         retry_times * 1000000.0 / (end_t - start_t));

    // GraphInfo graphInfo;
    // LOG(INFO) << "GetFullGraphInfo";
    // client->GetFullGraphInfo(graphInfo);
    // graphInfo.infos_[0];

    // BatchNodes batchNodes;
    // LOG(INFO) << "SampleBatchNodes";
    // client->SampleBatchNodes(NODE_TYPE, BATCH_SIZE, SampleStrategy::RANDOM, batchNodes);
    // batchNodes.printTo(LOG(WARNING));

    // std::vector<NodeFeature> r;
    // LOG(INFO) << "GetNodeFeature";
    // client->GetNodeFeature(batchNodes.node_ids, FEATURE_TYPE, r);

    // ***********

    // std::vector<IDNeighborPair> neighbors_;
    // LOG(INFO) << "SampleNeighbor";
    // client->SampleNeighbor(BATCH_SIZE, NODE_TYPE, NEIGHBOR_TYPE, BATCH_SIZE, neighbors_);

    // std::vector<NodeId> nodes;
    // LOG(INFO) << "RandomWalk";
    // client->RandomWalk(BATCH_SIZE, 10, nodes);
    return 0;
}