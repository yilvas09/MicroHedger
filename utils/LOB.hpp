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
    inline double bid() { return bids.size() ? bids.back().Price() : 0.0; }
    inline double ask() { return asks.size() ? asks.front().Price() : 0.0; }
    inline double mid() { return (ask() + bid()) * 0.5; }
    void AddLimitOrder(int s, double p, double v);
    int ContainsPrice(double p);
    int PriceLocation(int s, double p);
    void PrintLOB();
};

#endif
