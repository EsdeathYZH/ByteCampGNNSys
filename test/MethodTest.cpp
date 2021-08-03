#include "MethodTest.h"

#include <thread>

#include "../src/client/client_with_cache.h"
#include "../src/client/client_without_cache.h"
#include "../src/client/simple_cache.h"
#include "../src/utils/random.h"

using std::string;
using namespace ::ByteCamp;

void MethodTest::test_SampleBatchNodes(int id, int times) {
    string peerIP = "60.205.224.245";
    int port = 2021;
    std::vector<std::pair<string, int>> servers{std::make_pair(peerIP, port)};
    auto simple_cache = NewSimpleCache(100ll * (1 << 30));
    auto client = std::make_shared<ClientWithCache>(servers, simple_cache);

    time_t before = time(nullptr);
    RandomEngine *rd = new RandomEngine(0);
    std::vector<ByteGraph::IDFeaturePair> ttmp;

    while (times--) {
        ByteGraph::BatchNodes tmp;
        client->SampleBatchNodes(1, 1024, ByteGraph::SampleStrategy::RANDOM,0, tmp);
        assert(tmp.node_ids.size() == 1024);
    }
    time_t now = time(nullptr);
    printf("The test%d program has run for %d seconds\n", id, now - before);
}

// 采样指指定次数随机点邻居
void MethodTest::test_GetNodeNeighbors(int times) {
    string peerIP = "127.0.0.1";
    int port = 9090;
    std::vector<std::pair<string, int>> servers{std::make_pair(peerIP, port)};
    auto client = std::make_shared<ClientWithoutCache>(servers);

    time_t before = time(nullptr);
    RandomEngine *rd = new RandomEngine(0);
    std::vector<ByteGraph::IDFeaturePair> tmp;

    while (times--) {
        for (int i = 4; i < 7; ++i) client->GetNeighborsWithFeature(rd->RandInt(totalNodes), i, 0, tmp);
    }
    time_t now = time(nullptr);
    printf("The test program has run for %d seconds\n", now - before);
}

// 采样指定次数随机Author点的随机P类型(边类型）的一跳邻居并返回随机特征（P为参数)
void MethodTest::test_GetNeighborsWithFeature(int times) {
    string peerIP = "127.0.0.1";
    int port = 9090;
    std::vector<std::pair<string, int>> servers{std::make_pair(peerIP, port)};
    auto client = std::make_shared<ClientWithoutCache>(servers);

    time_t before = time(nullptr);
    RandomEngine *rd = new RandomEngine(0);
    std::vector<ByteGraph::IDFeaturePair> tmp;

    while (times--) {
        client->GetNeighborsWithFeature(rd->RandInt(totalNodes), rd->Uniform(4.0, 7.0), rd->RandInt(totalFeatures),
                                        tmp);
    }
    time_t now = time(nullptr);
    printf("The test program has run for %d seconds\n", now - before);
}

int main(int argc, char *argv[]) {
    int threadCount = atoi(argv[1]);
    long long testNum = atoll(argv[2]);

    long long amounts[] = {testNum / threadCount, testNum - testNum / threadCount * (threadCount - 1)};
    std::vector<std::thread> threadPool;

    for (int i = 0; i < threadCount; ++i) {
        threadPool.push_back(std::thread(MethodTest::test_SampleBatchNodes, i, amounts[threadCount - 1 == i]));
    }
    for (auto &i : threadPool) i.join();
}