#ifndef microhedger_player_dynamic_hedger_hpp
#define microhedger_player_dynamic_hedger_hpp

#include <vector>
#include "LOB.hpp"
#include "Option.hpp"

class DeltaHedger
{
private:
    double delta;
    double gamma;

    const double opt_pos;
    const double implied_vol;

    Bar outstanding_order;
    // inventories
    std::vector<Bar> stocks;
    std::vector<Option> options;

public:
    DeltaHedger(double pos, double iv) : opt_pos(pos), implied_vol(iv) {}
    ~DeltaHedger() {}

    inline double getOrderVolume() const { return outstanding_order.Volume(); }
    inline double getOrderPrice() const { return outstanding_order.Price(); }

    double Delta(double vol, const LOB &currLOB, double time) const;
    double Gamma(double vol, const LOB &currLOB, double time) const;
    bool IsMyOrderExecuted(const std::vector<std::vector<Bar>> &eos) const;

    void ResetGammaContract(double time, const LOB &currLOB);
    void ReCalcGreeks(double time, const LOB &currLOB);
    void PostOrder(double &p,                                           // [O] - price of hedger's order
                   double &v,                                           // [O] - volume of hedger's order
                   int &s,                                              // [O] - sign of hedger's order
                   const std::vector<std::vector<Bar>> &available_info, // [I] - executed orders
                   const LOB &currLOB,                                  // [I] - current lob
                   double t_q                                           // [I] - frac of current quarter / hour
    );
    void UpdateInventories(const std::vector<std::vector<Bar>> &eos);
};

#endif