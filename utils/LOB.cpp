#include <iostream>
#include "LOB.hpp"

LOB::LOB()
{
    bids.resize(0);
    asks.resize(0);
}

LOB::LOB(const std::vector<double> &aps, const std::vector<double> &avs,
         const std::vector<double> &bps, const std::vector<double> &bvs)
{
    for (int i = 0; i < aps.size(); i++)
    {
        asks.push_back(Bar(aps[i], avs[i]));
    }
    for (int i = 0; i < bps.size(); i++)
    {
        bids.push_back(Bar(bps[i], bvs[i]));
    }
}

// check whether the current LOB contains orders at price p; returns 1 (sell orders) or -1 (buy orders)
int LOB::ContainsPrice(double p) const
{
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
        std::vector<Bar> &bars_other_side = s > 0 ? bids : asks;
        std::vector<Bar>::iterator it = bars_other_side.begin() + PriceLocation(-s, p);
        auto &bar = *(it);
        bar.ExecuteAgainst(v);
        if (abs(bar.Volume() - 0.0) < __DBL_EPSILON__)
        {
            bars_other_side.erase(it);
            if (v > __DBL_EPSILON__) // if there is outstanding volume, we need to add it to existing lob
                AddLimitOrder(s, p, v);
        }
    }
}

// adjust the lob with an incoming market order of sign s (1: sell; -1: buy) and volume v
// return how many orders are executed at what price, and VWAP as a double
double LOB::AbsorbMarketOrder(std::vector<Bar> &eos,
                              double &v,
                              int s)
{
    if (s != -1 && s != 1)
    {
        std::cout << "LOB error: wrong sign for market orders. Must be -1 or 1." << std::endl;
        return 0.0;
    }
    eos.resize(0);
    double v_ttl = 0.0;
    double pos_ttl = 0.0;
    std::vector<Bar> &bars_other_side = -s > 0 ? asks : bids; // sell orders, otherside = bid; buy orders, otherside - asks
    // if otherside is ask, loop from begin to end; if bid, loop from end to begin
    while (v > __DBL_EPSILON__ && bars_other_side.size())
    {
        double orig_v = v;
        // if otherside is ask, execute from the beginning; otherwise from the end
        std::vector<Bar>::iterator it = -s > 0 ? bars_other_side.begin() : bars_other_side.end() - 1;
        auto &bar = *(it);
        bar.ExecuteAgainst(v);

        // record executed orders
        double exe_v = orig_v - v;
        v_ttl += exe_v;
        pos_ttl += exe_v * bar.Price();
        eos.push_back(Bar(bar.Price(), exe_v));

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
