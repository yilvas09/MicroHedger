#ifndef microhedger_utilities_lob_hpp
#define microhedger_utilities_lob_hpp

#include <vector>
#include "Bar.hpp"
#include "Utils.hpp"

class LOB
{
private:
    double decay_coef;
    bool safety_check;
    std::vector<Bar> bids; // all the buy orders with ascending prices
    std::vector<Bar> asks; // all the sell orders with ascending prices

public:
    LOB();
    LOB(const std::vector<double> &aps, const std::vector<double> &avs,
        const std::vector<double> &bps, const std::vector<double> &bvs);
    LOB(double _d_coef,
        const std::vector<double> &aps, const std::vector<double> &avs,
        const std::vector<double> &bps, const std::vector<double> &bvs);
    ~LOB() {}

    inline double bid() const { return bids.size() ? bids.back().Price() : -__DBL_MAX__; }
    inline double ask() const { return asks.size() ? asks.front().Price() : __DBL_MAX__; }
    inline double mid() const { return (ask() + bid()) * 0.5; }
    inline const Bar &Bid() const { return bids.size() ? bids.back() : theBidBar; }
    inline const Bar &Ask() const { return asks.size() ? asks.front() : theAskBar; }
    inline bool oneSideEmpty() const { return !asks.size() || !bids.size(); }
    inline bool bothSidesEmpty() const { return !asks.size() && !asks.size(); }
    inline void setSafetyCheck(bool state) { safety_check = state; }

    const Bar &getBarAt(int s, int pos) const;
    double getVolumeAt(int s, int pos) const;
    double getPriceAt(int s, int pos) const;
    
    double getTotalVolume(int s) const;

    int ContainsPrice(double p) const;
    int PriceLocation(int s, double p) const;
    void CheckUnsafeCall() const;
    void PrintLOB() const;

    void AddLimitOrder(int s, double p, double v);
    void CancelLimitOrder(int s, double p, double v);
    double AbsorbMarketOrder(std::vector<Bar> &eos, // [O] - executed orders
                             double &v,             // [I] and [O] - input volume and outstanding volume
                             int s);                // [I] - sign of the market order
    void AbsorbLimitOrder(std::vector<Bar> &eos,
                          double &v,
                          double p,
                          int s);
    void DecayOrders(double d_coef);
    void DecayOrders();
    std::vector<Bar> AbsorbGeneralOrder(OrderType o_type, // [I] - order type
                                        double p,         // [I] - price of order
                                        double v,         // [I] - volume of order
                                        int s             // [I] - sign of order
    );
};

#endif
