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

void DeltaHedger::ClearOrderAndInventories()
{
    // clear inventories from previous trading session
    stocks.resize(0);
    options.resize(0);
    // clear outstanding orders
    outstanding_order = Bar();
}

void DeltaHedger::ResetGammaContract(double time, const LOB &currLOB)
{
    ClearOrderAndInventories();
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

// hedger needs to know whether his own order posted, a bar with signed volume, is executed
// by going through all orders executed (eos) within this sub-trading interval
bool DeltaHedger::IsMyOrderExecuted(const std::vector<std::vector<Bar>> &eos) const
{
    // if there's no outstanding order, i.e. empty bar
    // the function does not go into this if-branch
    // and false is returned
    if (!outstanding_order.IsEmptyBar())
    {
        double oov = outstanding_order.Volume();
        if (abs(oov) < __DBL_EPSILON__)
            return true;
        for (auto &os : eos)
        {
            for (auto &o : os)
            {
                // compare order o with outstanding order
                bool same_p = abs(o.Price() - outstanding_order.Price()) < __DBL_EPSILON__;
                bool same_s = o.Volume() * outstanding_order.Volume() > 0;
                if (same_p && same_s)
                {
                    double ov = o.Volume();
                    double exe_v = ov > 0 ? std::min(ov, oov) : std::max(ov, oov);
                    oov -= exe_v;
                    if (abs(oov) < __DBL_EPSILON__)
                        return true;
                }
            }
        }
    }
    return false;
}

// based on execution results of this quarter hedger knows his state
// he then removes posted but unexecuted order (if any)
// and submit new order based on current LOB
void DeltaHedger::PostOrder(double &p,
                            double &v,
                            int &s,
                            const std::vector<std::vector<Bar>> &eos,
                            const LOB &currLOB,
                            double t_q)
{
    if (abs(delta) < __DBL_EPSILON__)
        return;
    bool isOrderExecuted = IsMyOrderExecuted(eos);
    if (outstanding_order.IsEmptyBar() || !isOrderExecuted)
    {
        // we post order if
        // 1. we haven't posted before (oustanding order not empty) and delta not zero
        // 2. OR the posted one is not executed
        double ba_spr = currLOB.ask() - currLOB.bid();
        s = delta > 0 ? 1 : -1; // sell stock if delta > 0, buy if < 0
        v = abs(delta);         // this can be replaced by v = delta when refactoring
        p = s > 0 ? currLOB.ask() : currLOB.bid();
        // from passive to aggresive
        if (t_q < 0.25)
            p += s * ba_spr; // p = pb - spr (buy) or pa + spr (sell)
        else if (t_q < 0.5)
            ; // do nothing, p = pb (buy) or pa (sell)
        else if (t_q < 0.75)
            p -= s * 0.5 * ba_spr; // p = pb + 0.5spr (buy) or pa - 0.5spr (sell)
        else
            p -= s * ba_spr; // p = pa (buy) or pb (sell)

        // record posted order
        outstanding_order = Bar(p, s * v);
    }
}

void DeltaHedger::UpdateInventories(const std::vector<std::vector<Bar>> &eos)
{
    bool isOrderExecuted = IsMyOrderExecuted(eos);
    if (!outstanding_order.IsEmptyBar() && !outstanding_order.IsEmptyVolume() && isOrderExecuted)
    {
        Bar bar(outstanding_order.Price(), -outstanding_order.Volume());
        stocks.push_back(bar);                                   // update inventories
        outstanding_order = Bar(outstanding_order.Price(), 0.0); // reset order
    }
}