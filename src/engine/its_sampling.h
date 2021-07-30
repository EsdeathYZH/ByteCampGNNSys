
#ifndef BYTEGRAPH_ENGINE_ITS_SAMPLING_H_
#define BYTEGRAPH_ENGINE_ITS_SAMPLING_H_

#include <vector>
#include "type.h"
#include "utils/random.h"

namespace Byte {

class ITSSampling {
public:
    explicit ITSSampling (WeightList& weights){
        this->sum_weight_ = 0.0;
        this->sum_weights_.resize(weights.sz);
        for (size_t i = 0; i < weights.sz; ++i) {
            this->sum_weight_ += weights.data[i * weights.stride];
            this->sum_weights_[i] = this->sum_weight_;
        }
        return;
    }

    size_t sample() const{
        return randomSelect(0, sum_weights_.size() - 1);
    }

    size_t randomSelect(size_t begin_pos, size_t end_pos) {
        float limit_begin = begin_pos == 0 ? 0 : sum_weights_[begin_pos - 1];
        float limit_end = sum_weights_[end_pos];
        float r = ThreadLocalRandom() * (limit_end - limit_begin) +
                    limit_begin;
        size_t low = begin_pos, high = end_pos, mid = 0;
        bool finish = false;
        while(low <= high && !finish) {
            mid = (low + high) / 2;
            float interval_begin = mid == 0 ? 0 : sum_weights_[mid - 1];
            float interval_end = sum_weights_[mid];
            if (interval_begin <= r && r < interval_end) {
                finish = true;
            } else if (interval_begin > r) {
                high = mid - 1;
            } else if (interval_end <= r) {
                low = mid + 1;
            }
        }
        return mid;
    }

private:
    std::vector<float> sum_weights_;
    float sum_weight_;
};

}

#endif
