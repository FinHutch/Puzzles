#include <utility>
#include <algorithm>
#include <cmath>
#include <vector>
#include <unordered_map>
#include "robot_baseball.h"
#include <iostream>
#include <iomanip>
std::pair<double, double> find_equilibrium_strategy(double ball, double strike, double p) {
    const double A = 4 * p + ball - strike - strike * p;
    const double B = strike - ball;
    const double C = ball;
    const double eps = 1e-12;
    // this comes from score = A * ts + B *(t+s) + C  where t is the probability of throwing a strike and
	// s is the probability of swinging the bat
    // this is finding the nash equilibrium of the game
    const double s = -B / A;
    if (s >= -eps && s <= 1.0 + eps) {
        const double t = std::clamp(s, 0.0, 1.0);
        return { A * t * t + B * 2 * t + C, t };
    }

    // it is never best to always swing or always throw a ball because its always exploitable
    // so I don't need to handle edge cases
    throw;
}




std::unordered_map<std::pair<int,int>, std::pair<double,double>, PairHash> get_strategies_vector(double p) {

    // I just find the best strategy for every state, the constants are the expected values of the next states, which 
    //can be worked out backwards
    std::unordered_map <std::pair<int, int>, std::pair<double,double>, PairHash> strategies;
    strategies[{3, 2}] = find_equilibrium_strategy(1, 0, p);
    strategies[{3, 1}] = find_equilibrium_strategy(1, strategies[{3, 2}].first, p);
    strategies[{3, 0}] = find_equilibrium_strategy(1, strategies[{3, 1}].first, p);
    strategies[{2, 2}] = find_equilibrium_strategy(strategies[{3, 2}].first, 0, p);
    strategies[{2, 1}] = find_equilibrium_strategy(strategies[{3, 1}].first, strategies[{2, 2}].first, p);
    strategies[{2, 0}] = find_equilibrium_strategy(strategies[{3, 0}].first, strategies[{2, 1}].first, p);
    strategies[{1, 2}] = find_equilibrium_strategy(strategies[{2, 2}].first, 0, p);
    strategies[{1, 1}] = find_equilibrium_strategy(strategies[{2, 1}].first, strategies[{1, 2}].first, p);
    strategies[{1, 0}] = find_equilibrium_strategy(strategies[{2, 0}].first, strategies[{1, 1}].first, p);
    strategies[{0, 2}] = find_equilibrium_strategy(strategies[{1, 2}].first, 0, p);
    strategies[{0, 1}] = find_equilibrium_strategy(strategies[{1, 1}].first, strategies[{0, 2}].first, p);
    strategies[{0, 0}] = find_equilibrium_strategy(strategies[{1, 0}].first, strategies[{0, 1}].first, p);
    return strategies;
}
double p_of_strike(double p, double t) {
    return (1 - p)* t* t + (1 - t) * t * 2;// because t and s are the same
}
double p_of_ball(double t) {
    return (1 - t) * (1 - t); // because t and s are the same
}
double probabilitiy_of_combination(std::vector<bool> values, std::unordered_map<std::pair<int, int>, std::pair<double, double>, PairHash>& strategies, double p) {
    std::pair<int, int> sofar = { 0,0 };
    //value 1 = ball
    //ball then strike
    double probability = 1;
    for (auto value : values) {
        if (value == 1) {
            probability *= p_of_ball(strategies[sofar].second);
            sofar.first += 1;
        }
        if (value == 0) {
            probability *= p_of_strike(p, strategies[sofar].second);
            sofar.second += 1;
        }
        
    }
    if (sofar.first != 3 || sofar.second != 2) throw;
    return probability;


}
double p_of_reaching_end(std::unordered_map<std::pair<int, int>, std::pair<double, double>, PairHash>& strategies, double p) {
    double p_reaching_end = 0;
    // all the ways to get 3 balls and 2 strikes
    std::vector<std::vector<bool>> values = {
    {1,1,1,0,0},
    {1,1,0,1,0},
    {1,1,0,0,1},
    {1,0,1,1,0},
    {1,0,1,0,1},
    {1,0,0,1,1},
    {0,1,1,1,0},
    {0,1,1,0,1},
    {0,1,0,1,1},
    {0,0,1,1,1}
    };
    for (auto value : values) {
        p_reaching_end += probabilitiy_of_combination(value, strategies, p);
    }
    return p_reaching_end;
}
void print_values(int divisions, double start, double end) {
    double maxvalue = 0;
    double lastvalue = 0;
    std::vector<double> results = {};
    for (double homerun_value = start; homerun_value <=end; homerun_value+=(end-start)/divisions) {
        if (homerun_value < 0.01) continue;
        auto strategy = get_strategies_vector(homerun_value);
        double value = p_of_reaching_end(strategy, homerun_value);
        std::cout << homerun_value << " " << value << std::endl;
		
    }
    
}

double find_best_value(double start, double end) {
    double divisions = 100;
    double size = end - start;

    double best_homerun_value = (start + end)/2;

    // unconventional way of finding teh maximum
    // I assumed there is one maximum because when I printed out values across a large range it looked like there
    // was a single peak
	// to solve the problem I called this function in main with start of 0.1 and end of 0.9
    double maxvalue;
    for (int i = 0; i < 50; i++) {
        maxvalue = 0;
        double start1 = best_homerun_value - size / 2;
        double end1 = best_homerun_value + size / 2;
        for (double homerun_value = start1; homerun_value <= end1; homerun_value += (end1 - start1) / divisions) {
            if (homerun_value < 0.01) continue;
            auto strategy = get_strategies_vector(homerun_value);
            double value = p_of_reaching_end(strategy, homerun_value);
            if (value > maxvalue) { maxvalue = value; best_homerun_value = homerun_value; };
        }
        std::cout << std::setprecision(20) << best_homerun_value << " " << maxvalue <<std::endl;
        size /= 10;
    }
    return maxvalue;
}

