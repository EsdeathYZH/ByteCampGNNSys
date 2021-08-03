#ifndef BYTEGRAPH_UTILS_H
#define BYTEGRAPH_UTILS_H

#include <fstream>
#include <string>
#include <vector>
#include <cmath>

namespace ByteCamp {

template <class T1, class T2>
inline std::vector<std::pair<T1, T2> > ReadConfig(const std::string& config_file) {
    T1 param1;
    T2 param2;
    std::vector<std::pair<T1, T2> > ret;
    std::ifstream config(config_file);
    while (config >> param1 >> param2) {
        ret.emplace_back(std::move(param1), std::move(param2));
    }
    return std::move(ret);
}

std::vector<int32_t> GetBatchSizeAccordingToWeights(const std::vector<int64_t>& weights, int32_t batchSize) {
    uint64_t totalWeight = 0;
    for (auto i : weights) totalWeight += i;
    std::vector<int32_t> result(weights.size());
    uint64_t totalCount = 0;
    for (int32_t i = 0; i < weights.size(); ++i) {
        result[i] = std::round(1.0 * weights[i] * batchSize / totalWeight);
        totalCount += result[i];
    }
    result[result.size() - 1] = batchSize - (totalCount - result.back());
    return result;
}

}  // namespace ByteCamp

#endif  // BYTEGRAPH_UTILS_H