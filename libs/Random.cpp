#include "Random.hpp"

Random::Random(int _seed, 
    double _vol_news, 
    double _order_intensity) :
    seed(_seed),
    norm_dist(std::normal_distribution<double>(0.0, _vol_news)),
    pois_dist(std::poisson_distribution<int>(_order_intensity))
{

}

double Random::GenerateShockedPrice(double p_prev) const
{
    return p_prev;
}

int Random::GenerateNumOrders() const
{
    return 0;
}

void Random::GenerateOrder(int &o_type, double &p, double &v) const
{
    
}