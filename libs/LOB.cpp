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

// getter functions to obtain a specific bar in the lob
const Bar &LOB::getBarAt(int s, int pos) const
{
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
    return getBarAt(s, pos).Volume();
}

double LOB::getPriceAt(int s, int pos) const
{
    return getBarAt(s, pos).Price();
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

// update LOB and add order based on order type
// return exercised limit order, sell/buy direction is marked by the sign of bar.volume
std::vector<Bar> LOB::AbsorbGeneralOrder(OrderType o_type, double p, double v, int s)
{
    std::vector<Bar> executed_orders;
    if (s == 0)
        return executed_orders;
    switch (o_type)
    {
    case LIMITORDER:
    {
        // need to keep track of whether exisiting orders are executed when the new order arrives
        LOB orig_lob(*this);
        // existing orders only get executed when an order on the other side comes
        const std::vector<Bar> &orig_bars_other_side = s > 0 ? orig_lob.bids : orig_lob.asks;
        int s_other_side = -s;
        AddLimitOrder(s, p, v);

        /* TODO: refactor this part so that executed orders are returned in AddLimitOrder
        compare orders on the other side and see whether anything has changed:
        1.  If current bars contains the bar at the same side,
            either volume is not touched, or the volume is touched (exercised);
        2.  If current bars contains the bar at the opposite side,
            or doesn't contain at all,
            then the bar is already executed in full.
        */
        for (auto o_bar : orig_bars_other_side)
        {
            double orig_p = o_bar.Price();
            double orig_v = o_bar.Volume();
            int contains_p = ContainsPrice(orig_p);
            if ((contains_p * s_other_side == 0)    // no bar at this price
                || (contains_p * s_other_side < 0)) // bar with same price at the opposite
                executed_orders.push_back(o_bar);
            else // bar on the same side
            {
                double curr_v = getVolumeAt(s_other_side, PriceLocation(s_other_side, orig_p));
                double diff_v = orig_v - curr_v;
                if (diff_v > __DBL_EPSILON__)
                    executed_orders.push_back(Bar(orig_p, s_other_side * diff_v));
            }
        }
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