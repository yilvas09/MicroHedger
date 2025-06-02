#ifndef microhedger_utilities_random_hpp
#define microhedger_utilities_random_hpp

#include <random>
#include "Utils.hpp"

class Random
{
private:
    int seed;
    std::default_random_engine generator;
    std::normal_distribution<double> norm_dist;
    std::poisson_distribution<int> pois_dist;

public:
    Random(int _seed,
           double _vol_news,
           double _order_intensity);
    ~Random() {}

    double GenerateShockedPrice(double p_prev) const;
    int GenerateNumOrders() const;
    void GenerateOrder(OrderType &o_type, double &p, double &v, int &s) const;
};

#endif