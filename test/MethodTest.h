#ifndef METHOD_TEST_H
#define METHOD_TEST_H
#include <stdint.h>

class MethodTest{
public:

    const static int64_t totalNodes = 121751666 + 122383112 + 25721;
    const static int64_t totalFeatures = 768;
    void static test_SampleBatchNodes(int id, int times);
    // 采样指指定次数随机点邻居
    void static test_GetNodeNeighbors(int times);
    // 采样指定次数随机Author点的随机P类型(边类型）的一跳邻居并返回随机特征（P为参数)
    void static test_GetNeighborsWithFeature(int times);
};

#endif