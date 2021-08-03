#include <fstream>
#include <string>
#include <vector>

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