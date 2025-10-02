#pragma once
#include <utility>
#include <cmath>
#include <limits>
#include <vector>
#include <unordered_map>
struct PairHash {
    // this hash function is from chatgpt.
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const noexcept {
        std::size_t h1 = std::hash<T1>{}(p.first);
        std::size_t h2 = std::hash<T2>{}(p.second);
        return h1 ^ (h2 + 0x9e3779b97f4a7c15ull + (h1 << 6) + (h1 >> 2));
    }
};
std::pair<double,double> find_equilibrium_strategy(double k1, double k2, double p);

std::unordered_map<std::pair<int, int>, std::pair<double,double>, PairHash> get_strategies_vector(double p);

double p_of_reaching_end(std::unordered_map<std::pair<int, int>, std::pair<double, double>, PairHash>& strategies, double p);
void print_values(int divisions, double start, double end);
double find_best_value(double start, double end);

