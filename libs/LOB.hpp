#ifndef microhedger_utilities_lob_hpp
#define microhedger_utilities_lob_hpp

#include <vector>
#include "Bar.hpp"

class LOB
{
private:
    std::vector<Bar> bids; // all the buy orders with ascending prices
    std::vector<Bar> asks; // all the sell orders with ascending prices

public:
    LOB();
    LOB(const std::vector<double> &aps, const std::vector<double> &avs,
        const std::vector<double> &bps, const std::vector<double> &bvs);
    ~LOB() {}

    inline double bid() const { return bids.size() ? bids.back().Price() : 0.0; }
    inline double ask() const { return asks.size() ? asks.front().Price() : 0.0; }
    inline double mid() const { return (ask() + bid()) * 0.5; }
    inline bool oneSideEmpty() const { return !asks.size() || !bids.size(); }

    void AddLimitOrder(int s, double p, double v);
    int ContainsPrice(double p) const;
    int PriceLocation(int s, double p) const;
    double AbsorbMarketOrder(std::vector<Bar> &eos, // [O] - executed orders
                             double &v,             // [I] and [O] - input volume and outstanding volume
                             int s);
    void PrintLOB() const;
};

#endif
