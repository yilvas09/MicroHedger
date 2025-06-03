#ifndef microhedger_player_dynamic_hedger_hpp
#define microhedger_player_dynamic_hedger_hpp

#include <vector>
#include "Bar.hpp"

class DeltaHedger
{
private:
    double delta;
    double gamma;

public:
    DeltaHedger(double d, double g) : delta(d), gamma(g) {}
    ~DeltaHedger() {}

    inline double Delta() const { return delta; }
    inline double Gamma() const { return gamma; }

    void ResetGammaContract();
    void ReCalcDelta();
    void Act(double &p,                             // [O] - price of hedger's order
             double &v,                             // [O] - volume of hedger's order
             int &s,                                // [O] - sign of hedger's order
             const std::vector<std::vector<Bar>> &available_info // [I] - executed orders
    );
};

#endif