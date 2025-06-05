#include "Random.hpp"

Random::Random(int _seed,
               double _vol_news,
               double _order_intensity,
               double _prob_otype,
               double _prob_info,
               double _vol_min, double _vol_max,
               double _mean_spread, double _vol_spread)
    : seed(_seed),
      vol_news(_vol_news),
      order_intensity(_order_intensity),
      prob_otype(_prob_otype),
      prob_info(_prob_info),
      vol_min(_vol_min),
      vol_max(_vol_max),
      mean_spread(_mean_spread),
      vol_spread(_vol_spread),
      prob_sign(0.5),
      generator(std::default_random_engine()),
      norm_dist_p_shock(std::normal_distribution<double>(0.0, vol_news)),
      pois_dist_onum(std::poisson_distribution<int>(order_intensity)),
      ber_dist_otype(std::bernoulli_distribution(prob_otype)),
      ber_dist_info(std::bernoulli_distribution(prob_info)),
      uni_dist_v_mm(std::uniform_real_distribution<double>(vol_min, vol_max)),
      norm_dist_p_mm(std::normal_distribution<double>(mean_spread, vol_spread)),
      ber_dist_sign(std::bernoulli_distribution(prob_sign))
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
    o_type = ber_dist_otype(generator) >= prob_otype ? MARKETORDER : LIMITORDER;
    v = uni_dist_v_mm(generator);
    bool informed = ber_dist_info(generator);
    switch (o_type)
    {
    case MARKETORDER:
    {
        if (informed)
            s = p_mid > p_fund ? 1 : -1; // sell when current price > fundamentals; buy otherwise
        else
            s = ber_dist_sign(generator) > prob_sign ? 1 : -1;
        break;
    }
    case LIMITORDER:
    {
        s = ber_dist_sign(generator) > prob_sign ? 1 : -1;
        // informed market makers use fundamental price as reference,
        // while uninformed ones use mid prices
        p = (informed ? p_fund : p_mid) + s * norm_dist_p_mm(generator);
        break;
    }
    default:
        throw std::invalid_argument("Invalid order type.");
    }
}