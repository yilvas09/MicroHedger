#include "Option.hpp"

Option::Option(OptionType _type,
               double _t_incept,
               double _t_mat,
               double _strike)
    : type(_type),
      t_incept(_t_incept),
      t_mat(_t_mat),
      strike(_strike)
{
}

double Option::Delta(double vol, double spot, double t) const
{
    double delta = 0.0;
    switch (type)
    {
    case STRADDLE:
    {
        delta = (spot - strike) / vol / sqrt(t_mat - t);
        delta = 2 * Utils::normalCDF(delta) - 1;
        break;
    }
    default:
        throw std::invalid_argument("This option type is not implemented.");
    }
    return delta;
}

double Option::Gamma(double vol, double spot, double t) const
{
    double gamma = 0.0;
    switch (type)
    {
    case STRADDLE:
    {
        double inv_vol_t2mat = 1. / vol / sqrt(t_mat - t);
        gamma = (spot - strike) * inv_vol_t2mat;
        gamma = 2 * Utils::normalPDF(gamma) * inv_vol_t2mat / spot;
        break;
    }
    default:
        throw std::invalid_argument("This option type is not implemented.");
    }
    return gamma;
}