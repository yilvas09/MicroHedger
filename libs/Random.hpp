#ifndef microhedger_utilities_random_hpp
#define microhedger_utilities_random_hpp

#include <random>
#include "Utils.hpp"

struct RandomInfo
{
    int seed;
    double vol_news;
    double order_intensity;
    double prob_otype;
    double prob_info;
    double v_min, v_max;
    double mean_spread, vol_spread;
    double prob_sign;

    RandomInfo(int _seed,
               double _vol_news,
               double _order_intensity,
               double _prob_otype,
               double _prob_info,
               double _v_min, double _v_max,
               double _mean_spread, double _vol_spread,
               double _prob_sign)
        : seed(_seed),
          vol_news(_vol_news),
          order_intensity(_order_intensity),
          prob_otype(_prob_otype),
          prob_info(_prob_info),
          v_min(_v_min),
          v_max(_v_max),
          mean_spread(_mean_spread),
          vol_spread(_vol_spread),
          prob_sign(_prob_sign)
    {
    }

    RandomInfo(int _seed,
               const RandomInfo &ri)
        : RandomInfo(ri)
    {
        seed = _seed;
    }

    static void GenerateScenarios(std::vector<RandomInfo> &scens,
                                  const Parameter &param_name,
                                  const std::vector<double> &range,
                                  const RandomInfo &ri_template);
};

class Random
{
private:
    std::default_random_engine generator;
    std::normal_distribution<double> norm_dist_p_shock;   // news shocks on fundamental price
    std::poisson_distribution<int> pois_dist_onum;        // number of orders at each subinterval
    std::bernoulli_distribution ber_dist_otype;           // type of external orders
    std::bernoulli_distribution ber_dist_info;            // portions of informed/uninformed external orders
    std::uniform_real_distribution<double> uni_dist_v_mm; // volumes of external orders
    std::normal_distribution<double> norm_dist_p_mm;      // spread prices of external orders from market makers
    std::bernoulli_distribution ber_dist_sign;            // sign of external orders

public:
    Random(int _seed,
           double _vol_news,
           double _order_intensity,
           double _prob_otype,
           double _prob_info,
           double _v_min, double _v_max,
           double _mean_spread, double _vol_spread,
           double _prob_sign = 0.5);
    Random(const RandomInfo &ri);
    ~Random() {}

    double GenerateShockedPrice(double p_prev);
    int GenerateNumOrders();
    void GenerateOrder(OrderType &o_type, // [O] - LIMITORDER or MARKETORDER
                       double &p,         // [O] - price of the external order
                       double &v,         // [O] - volume of the external order
                       int &s,            // [O] - sign of the external order
                       double p_mid,      // [I] - mid price of the LOB for uninformed agents
                       double p_fund);    // [I] - current fundamental price for informed agents
};

#endif
