#include <iostream>
#include "LOB.hpp"

LOB::LOB()
{
    bids.resize(0);
    asks.resize(0);
    decay_coef = 0.0;
    safety_check = false;
}

LOB::LOB(const std::vector<double> &aps, const std::vector<double> &avs,
         const std::vector<double> &bps, const std::vector<double> &bvs)
    : LOB()
{
    if (aps.size() != avs.size() || bps.size() != bvs.size())
        throw std::invalid_argument("Price and volume vectors must have same size");
    for (int i = 0; i < 2; i++)
    {
        std::vector<double> ps_tmp = i == 0 ? aps : bps;
        std::vector<double> vs_tmp = i == 0 ? avs : bvs;
        std::vector<Bar> &bars = i == 0 ? asks : bids;
        // sort ps_tmp while organising vs_tmp so that they are in the same order
        Utils::sortPairedVectors(ps_tmp, vs_tmp);
        for (int j = 0; j < ps_tmp.size(); j++)
            bars.push_back(Bar(ps_tmp[j], vs_tmp[j]));
    }
}

LOB::LOB(double _d_coef,
         const std::vector<double> &aps, const std::vector<double> &avs,
         const std::vector<double> &bps, const std::vector<double> &bvs)
    : LOB(aps, avs, bps, bvs)
{
    decay_coef = _d_coef;
}

// getter functions to obtain a specific bar in the lob
const Bar &LOB::getBarAt(int s, int pos) const
{
    CheckUnsafeCall();
    if (!s)
        throw std::invalid_argument("Invalid sign; must be non-zero integer.");
    const std::vector<Bar> &bars = s > 0 ? asks : bids;
    int n_bars = static_cast<int>(bars.size());
    if (pos >= n_bars || pos < -n_bars)
        throw std::invalid_argument("Invalid bar position; out of boundary.");
    std::vector<Bar>::const_iterator it_start = pos >= 0 ? bars.begin() : bars.end();
    return *(it_start + pos);
}

double LOB::getVolumeAt(int s, int pos) const
{
    CheckUnsafeCall();
    return getBarAt(s, pos).Volume();
}

double LOB::getPriceAt(int s, int pos) const
{
    CheckUnsafeCall();
    return getBarAt(s, pos).Price();
}

void LOB::CheckUnsafeCall() const
{
    if (safety_check && oneSideEmpty())
        throw std::out_of_range("One side of the LOB is empty. Potential malfunction under market failure.");
}

// check whether the current LOB contains orders at price p; returns 1 (sell orders) or -1 (buy orders)
int LOB::ContainsPrice(double p) const
{
    CheckUnsafeCall();
    if (p > bid() && p < ask())
        return 0;
    const int sign = p <= bid() ? -1 : 1;
    const std::vector<Bar> &bars = sign > 0 ? asks : bids;
    for (auto bar : bars)
    {
        if (abs(bar.Price() - p) < __DBL_EPSILON__)
            return sign;
    }
    return 0;
}

// return the location of a price in one side of the lob (s = 1: asks; s = -1: bids), in ascending order
int LOB::PriceLocation(int s, double p) const
{
    CheckUnsafeCall();
    if (s == 0)
        return -1;

    const std::vector<Bar> &bars = s > 0 ? asks : bids;
    int location = 0;
    for (auto bar : bars)
    {
        if (bar.Price() >= p - __DBL_EPSILON__)
            break;
        location++;
    }
    return location;
}

// add a limit order of price p and volume v, with sign s (s = 1, an ask/sell order; s = -1, a bid/buy order)
void LOB::AddLimitOrder(int s, double p, double v)
{
    CheckUnsafeCall();
    if (s == 0)
        return;
    int state = ContainsPrice(p); // 0 (no orders at p), 1 (sell orders at p), -1 (buy orders at p)
    state *= s;                   // 0 (insert new prices), 1 (increase vol in LOB), -1 (execute against existing bar)
    if (state == 0)               // insert a new bar with price p
    {
        std::vector<Bar> &bars = s > 0 ? asks : bids;
        std::vector<Bar>::iterator it = bars.begin() + PriceLocation(s, p);
        bars.insert(it, Bar(p, v));
    }
    else if (state > 0) // already exists a bar at price p on the same side of book -> add volume to existing bar
    {
        std::vector<Bar> &bars = s > 0 ? asks : bids;
        std::vector<Bar>::iterator it = bars.begin() + PriceLocation(s, p);
        auto &bar = *(it);
        bar.AddVolumesBy(v);
    }
    else // exists a bar at price p on the other side of the book -> execute against the bar
    {
        // safety measure: make sure 2 sides never cross
        if ((s > 0 && p < bid()) || (s < 0 && p > ask()))
            throw std::invalid_argument("Cannot post sell/buy limit order greater than bid/ask price!");

        std::vector<Bar> &bars_other_side = s > 0 ? bids : asks;
        std::vector<Bar>::iterator it = bars_other_side.begin() + PriceLocation(-s, p);
        auto &bar = *(it);
        bar.ExecuteAgainst(v);
        if (abs(bar.Volume()) < __DBL_EPSILON__)
        {
            bars_other_side.erase(it);
            if (v > __DBL_EPSILON__) // if there is outstanding volume, we need to add it to existing lob
                AddLimitOrder(s, p, v);
        }
    }
}

// cancel a limit order of price and and volume v
void LOB::CancelLimitOrder(int s, double p, double v)
{
    CheckUnsafeCall();
    int state = ContainsPrice(p); // 0 (no orders at p), 1 (sell orders at p), -1 (buy orders at p)
    if (s * state <= 0)           // if no orders at the specified side, do nothing
        return;
    std::vector<Bar> &bars = s > 0 ? asks : bids;
    std::vector<Bar>::iterator it = bars.begin() + PriceLocation(s, p);
    auto &bar = *(it);
    bar.AddVolumesBy(-v);
    if (bar.Volume() < __DBL_EPSILON__)
        bars.erase(it);
}

// adjust the lob with an incoming market order of sign s (1: sell; -1: buy) and volume v
// return how many orders are executed at what price, and VWAP as a double
double LOB::AbsorbMarketOrder(std::vector<Bar> &eos,
                              double &v,
                              int s)
{
    CheckUnsafeCall();
    if (s != -1 && s != 1)
        throw std::invalid_argument("Invalid sign for market orders. Must be -1 or 1.");
    eos.resize(0);
    double v_ttl = 0.0;
    double pos_ttl = 0.0;
    int s_other_side = -s;
    std::vector<Bar> &bars_other_side = s_other_side > 0 ? asks : bids; // sell orders, otherside = bid; buy orders, otherside - asks
    // if otherside is ask, loop from begin to end; if bid, loop from end to begin
    while (v > __DBL_EPSILON__ && bars_other_side.size())
    {
        double orig_v = v;
        // if otherside is ask, execute from the beginning; otherwise from the end
        std::vector<Bar>::iterator it = s_other_side > 0 ? bars_other_side.begin() : bars_other_side.end() - 1;
        auto &bar = *(it);
        bar.ExecuteAgainst(v);

        // record executed orders
        double exe_v = orig_v - v;
        v_ttl += exe_v;
        pos_ttl += exe_v * bar.Price();
        eos.push_back(Bar(bar.Price(), s_other_side * exe_v));

        if (bar.Volume() < __DBL_EPSILON__)
            bars_other_side.erase(it);
    }
    return abs(v_ttl) > __DBL_EPSILON__ ? pos_ttl / v_ttl : 0.0;
}

// pretty print the lob in the following format
/*
    price   1.1 1.5 3.2 4.1 4.5 5.2
    volume  -1  -4  -2   2   4   1
*/
void LOB::PrintLOB() const
{
    std::string title = " Current limit order book ";
    std::string p_row = "price\t";
    std::string v_row = "volume\t";
    for (auto bar : bids)
    {
        p_row += boost::str(boost::format("%1$.1f\t") % bar.Price());
        v_row += boost::str(boost::format("%1$.1f\t") % -bar.Volume());
    }
    for (auto bar : asks)
    {
        p_row += boost::str(boost::format("%1$.1f\t") % bar.Price());
        v_row += boost::str(boost::format("%1$.1f\t") % bar.Volume());
    }
    int length = std::max(p_row.length(), v_row.length());
    int nchar1 = std::max(int(length - title.size()) / 2, 0);
    int nchar2 = std::max(int(length - nchar1 - title.size()), 0);
    title = std::string(nchar1, '=') + title + std::string(nchar2, '=');
    std::cout << title << std::endl;
    std::cout << p_row << std::endl;
    std::cout << v_row << std::endl;
}

// decay resting orders in current LOB with a decay coefficient
void LOB::DecayOrders(double d_coef)
{
    CheckUnsafeCall();
    double p_mid = mid();
    for (int i = 0; i < 2; i++)
    {
        auto &bars = i == 0 ? asks : bids;
        for (auto &bar : bars)
        {
            double d_factor = exp(-d_coef * pow(p_mid - bar.Price(), 2));
            // v = v * a = v + (a - 1) * v
            bar.AddVolumesBy((d_factor - 1) * bar.Volume());
        }
    }
}

void LOB::DecayOrders()
{
    DecayOrders(decay_coef);
}

// update LOB and add order based on order type
// return exercised limit order, sell/buy direction is marked by the sign of bar.volume
std::vector<Bar> LOB::AbsorbGeneralOrder(OrderType o_type, double p, double v, int s)
{
    CheckUnsafeCall();
    std::vector<Bar> executed_orders;
    if (s == 0)
        return executed_orders;
    switch (o_type)
    {
    case LIMITORDER:
    {
        AbsorbLimitOrder(executed_orders, v, p, s);
        break;
    }
    case MARKETORDER:
    {
        AbsorbMarketOrder(executed_orders, v, s);
        break;
    }
    default:
        break;
    }
    return executed_orders;
}

void LOB::AbsorbLimitOrder(std::vector<Bar> &eos,
                           double &v,
                           double p,
                           int s)
{
    CheckUnsafeCall();
    if (s == 0)
        return;
    // handle illegal LO as market orders
    bool illegalLO = (s > 0 && p <= bid()) || (s < 0 && p >= ask());
    while (illegalLO && v > __DBL_EPSILON__)
    {
        double p_lo = -s > 0 ? ask() : bid();
        double v_lo = getVolumeAt(-s, PriceLocation(-s, p_lo));
        double v_exe = std::min(v_lo, v);
        v -= v_exe;
        AbsorbMarketOrder(eos, v_exe, s);
        illegalLO = (s > 0 && p <= bid()) || (s < 0 && p >= ask());
    }
    // exit the loop only when the order is fully executed or partially executed and legal
    if (v > __DBL_EPSILON__)
    {
        std::vector<Bar> &bars = s > 0 ? asks : bids;
        std::vector<Bar>::iterator it = bars.begin() + PriceLocation(s, p);
        auto &bar = *(it);
        if (ContainsPrice(p))
            bar.AddVolumesBy(v);
        else
            bars.insert(it, Bar(p, v));
    }
}