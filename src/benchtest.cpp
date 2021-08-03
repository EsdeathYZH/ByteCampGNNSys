#include <glog/logging.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#include <algorithm>
#include <thread>

#include "client/client_without_cache.h"
#include "../src/utils/utils.h"

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

void test_GetNeighborsWithFeature(const std::vector<std::pair<string, int>>& servers, const NodeId& NODE_ID_MAX,
                                  const EdgeType& NEIGHBOR_TYPE_MAX, const FeatureType& FEATURE_TYPE_MAX) {
    auto client = std::make_shared<ClientWithoutCache>(servers);

    std::vector<IDFeaturePair> neighbors;

    for (NodeId node_id = 0; node_id < NODE_ID_MAX; node_id++) {
        for (EdgeType edge_type = 0; edge_type < NEIGHBOR_TYPE_MAX; edge_type++) {
            for (FeatureType feature_type = 0; feature_type < FEATURE_TYPE_MAX; feature_type++) {
                client->GetNeighborsWithFeature(node_id, edge_type, feature_type, neighbors);
            }
        }
    }
}

int main(int argc, char** argv) {
    // Initialize Google’s logging library.
    google::InitGoogleLogging(argv[0]);
    google::SetLogDestination(google::INFO, "/tmp/log/INFO_");
    // google::SetStderrLogging(google::INFO);
    FLAGS_logtostderr = true;

    if (argc < 6) {
        printf("Usage: benchtest <node_id> <neighbor_type> <feature_type> <server_list> <thread_nums>");
        return 0;
    }

    const NodeId NODE_ID_MAX = (NodeId) atoll(argv[1]);
    const EdgeType NEIGHBOR_TYPE_MAX = (EdgeType) atoll(argv[2]);
    const FeatureType FEATURE_TYPE_MAX = (FeatureType) atoll(argv[3]);
    const char* server_list_file = argv[4];
    const int32_t thr_num = atoi(argv[5]);
    int64_t retry_times = NODE_ID_MAX * NEIGHBOR_TYPE_MAX * FEATURE_TYPE_MAX * thr_num;

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
    printf("benchtest start\n");

    for (int i = 0; i < thr_num; ++i) {
        thrs.emplace_back(
            // thread(bind(test_GetNeighborsWithFeature, servers, NODE_ID_MAX, NEIGHBOR_TYPE_MAX, FEATURE_TYPE_MAX)));
            thread(bind(test_SampleBatchNodes, servers, NODE_ID_MAX, NEIGHBOR_TYPE_MAX, FEATURE_TYPE_MAX)));
    }
    for (auto& th : thrs) {
        th.join();
    }

    printf("benchtest end\n");
    int64_t end_t = get_wall_time();

    fprintf(stderr, "test_GetNeighborsWithFeature %lld times cost = %f ms, qps = %f\n", retry_times,
            (end_t - start_t) / 1000.0, retry_times * 1000000.0 / (end_t - start_t));

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