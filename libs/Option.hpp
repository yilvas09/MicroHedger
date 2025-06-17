#ifndef microhedger_utilities_option_hpp
#define microhedger_utilities_option_hpp

#include "Utils.hpp"

class Option
{
private:
    OptionType type;
    double t_incept;
    double t_mat;
    double strike;
    double position;

public:
    Option(OptionType _type,
           double _t_incept,
           double _t_mat,
           double _strike,
           double _pos);
    Option() : Option(INVALID, 0.0, 0.0, 0.0, 0.0) {}
    ~Option() {}

    double Delta(double vol, double spot, double t) const;
    double Gamma(double vol, double spot, double t) const;
};

#endif