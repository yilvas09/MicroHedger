#include "Random.hpp"

void RandomInfo::GenerateScenarios(std::vector<RandomInfo> &scens,
                                   const Parameter &param_name,
                                   const std::vector<double> &range,
                                   const RandomInfo &ri_template)
{
    const int n = range.size();
    scens.resize(n, ri_template);
    for(int i = 0; i<n;i++)
    {
        RandomInfo &scen = scens[i];
        double val = range[i];
        switch (param_name)
        {
        case RANDOM_SEED:
            scen.seed = val;
            break;
        case VOLATILITY_FUNDAMENTAL:
            scen.vol_news = val;
            break;
        case ORDER_INTENSITY:
            scen.order_intensity = val;
            break;
        case PROB_LIMITORDER:
            scen.prob_otype = val;
            break;
        case PROB_INFORMED:
            scen.prob_info = val;
            break;
        case PROB_BUY:
            scen.prob_sign = val;
            break;
        case VOLUME_MIN:
            scen.v_min = val;
            break;
        case VOLUME_MAX:
            scen.v_max = val;
            break;
        case SPREAD_MEAN:
            scen.mean_spread = val;
            break;
        case SPREAD_VOLATILITY:
            scen.vol_spread = val;
            break;
        default:
            throw std::invalid_argument("This parameter is not supported yet.");
            break;
        }
    }

}

Random::Random(int _seed,
               double _vol_news,
               double _order_intensity,
               double _prob_otype,
               double _prob_info,
               double _v_min, double _v_max,
               double _mean_spread, double _vol_spread,
               double _prob_sign)
    : generator(std::default_random_engine(_seed)),
      norm_dist_p_shock(std::normal_distribution<double>(0.0, _vol_news)),
      pois_dist_onum(std::poisson_distribution<int>(_order_intensity)),
      ber_dist_otype(std::bernoulli_distribution(_prob_otype)),
      ber_dist_info(std::bernoulli_distribution(_prob_info)),
      uni_dist_v_mm(std::uniform_real_distribution<double>(_v_min, _v_max)),
      norm_dist_p_mm(std::normal_distribution<double>(_mean_spread, _vol_spread)),
      ber_dist_sign(std::bernoulli_distribution(_prob_sign))
{
}

Random::Random(const RandomInfo &ri)
    : Random(ri.seed, ri.vol_news, ri.order_intensity, ri.prob_otype,
             ri.prob_info, ri.v_min, ri.v_max, ri.mean_spread, ri.vol_spread,
             ri.prob_sign)
{
}

double Random::GenerateShockedPrice(double p_prev)
{
    return p_prev + norm_dist_p_shock(generator);
}

int Random::GenerateNumOrders()
{
    return pois_dist_onum(generator);
}

void Random::GenerateOrder(OrderType &o_type, // [O] - LIMITORDER or MARKETORDER
                           double &p,         // [O] - price of the external order
                           double &v,         // [O] - volume of the external order
                           int &s,            // [O] - sign of the external order
                           double p_mid,      // [I] - mid price of the LOB for uninformed agents
                           double p_fund)     // [I] - current fundamental price for informed agents
{
    o_type = ber_dist_otype(generator) ? LIMITORDER : MARKETORDER;
    v = uni_dist_v_mm(generator);
    bool informed = ber_dist_info(generator);
    double p_sign = ber_dist_sign.param().p();
    switch (o_type)
    {
    case MARKETORDER:
    {
        if (informed)
            s = p_mid > p_fund ? 1 : -1; // sell when current price > fundamentals; buy otherwise
        else
            s = ber_dist_sign(generator) > p_sign ? 1 : -1;
        break;
    }
    case LIMITORDER:
    {
        s = ber_dist_sign(generator) > p_sign ? 1 : -1;
        // informed market makers use fundamental price as reference,
        // while uninformed ones use mid prices
        p = (informed ? p_fund : p_mid) - s * norm_dist_p_mm(generator);
        break;
    }
    default:
        throw std::invalid_argument("Invalid order type.");
    }
}