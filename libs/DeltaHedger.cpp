#include "DeltaHedger.hpp"

double DeltaHedger::Delta(double vol, const LOB &currLOB, double time) const
{
    double d_opt = 0.0, d_stk = 0.0;
    for (auto &opt : options)
        d_opt += opt.Delta(vol, currLOB.mid(), time);
    for (auto &bar : stocks)
        d_stk += bar.Volume(); // signed volume
    return d_opt + d_stk;
}

double DeltaHedger::Gamma(double vol, const LOB &currLOB, double time) const
{
    double g_opt = 0.0;
    for (auto &opt : options)
        g_opt += opt.Gamma(vol, currLOB.mid(), time);
    return g_opt;
}

void DeltaHedger::ResetGammaContract(double time, const LOB &currLOB)
{
    // clear inventories from previous trading session
    stocks.resize(0);
    options.resize(0);
    // buy/sell ATM straddles maturing 2 days later 
    double spot = currLOB.mid();
    options.push_back(Option(STRADDLE, time, time + 2., spot, opt_pos));
    // update greeks
    ReCalcGreeks(time, currLOB);
}

void DeltaHedger::ReCalcGreeks(double time, const LOB &currLOB)
{
    delta = Delta(implied_vol, currLOB, time);
    gamma = Gamma(implied_vol, currLOB, time);
}

void DeltaHedger::Act(double &p, double &v, int &s, const std::vector<std::vector<Bar>> &info)
{
}