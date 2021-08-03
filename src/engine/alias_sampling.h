
#ifndef BYTEGRAPH_ENGINE_ALIAS_SAMPLING_H_
#define BYTEGRAPH_ENGINE_ALIAS_SAMPLING_H_

#include <vector>
#include <numeric>
#include "graph/type.h"
#include "utils/random.h"

namespace Byte {

class ALIASSampling {
public:
	explicit ALIASSampling (WeightList& weights){
		//normalize
		double sum_weight = 0.0;
        std::vector<double> norm_weights(weights.sz);
        for (size_t i = 0; i < weights.sz; i++) {
            sum_weight += weights.data[i * weights.stride];
            norm_weights[i] = weights.data[i * weights.stride];
        }

		for (size_t i = 0; i < norm_weights.size(); i++) {
			norm_weights[i] /= sum_weight;
		}

		prob.resize(norm_weights.size());
		alias.resize(norm_weights.size());
		std::vector<int64_t> small, large;
		double avg = 1 / static_cast<double>(norm_weights.size());
		for (size_t i = 0; i < norm_weights.size(); i++) {
			if (norm_weights[i] > avg) {
				large.push_back(i);
			} else {
				small.push_back(i);
			}
		}

		int64_t less, more;
		while (large.size() > 0 && small.size() > 0) {
			less = small.back();
			small.pop_back();
			more = large.back();
			large.pop_back();
			prob[less] = norm_weights[less] * norm_weights.size();
			alias[less] = more;
			norm_weights[more] = norm_weights[more] + norm_weights[less] - avg;
			if (norm_weights[more] > avg) {
				large.push_back(more);
			} else {
				small.push_back(more);
			}

		}  // while (large.size() > 0 && small.size() > 0)
		while (small.size() > 0) {
			less = small.back();
			small.pop_back();
			prob[less] = 1.0;
		}

		while (large.size() > 0) {
			more = large.back();
			large.pop_back();
			prob[more] = 1.0;
		}
	}

  	int64_t sample() const {
		int64_t column = nextLong(prob.size());
		bool coinToss = ThreadLocalRandom() < prob[column];
		return coinToss ? column : alias[column];
	}

	size_t getSize() const {
		return prob.size();
	}

	std::string showData() const{
		std::string result = "prob: {\n";
		for (auto& prob_item : prob) {
			result += std::to_string(prob_item);
			result += "\n";
		}
		result += "}\n";

		result += "alias: {\n";
		for (auto& alias_item : alias) {
			result += std::to_string(alias_item);
			result += "\n";
		}
		result += "}\n";

		return result;
	}

	std::vector<double> prob;
	std::vector<int> alias;

	int64_t nextLong(int64_t n) const {
		return floor(n * ThreadLocalRandom());
	}
};

}

#endif
