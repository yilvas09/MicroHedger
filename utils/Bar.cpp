#include "Bar.hpp"

int Bar::ExecuteAgainst(double &v)
{
    double executed_vol = std::min(volume, v); // between 0 and v
    volume -= executed_vol;                    // outstanding volume of current bar, between 0 and original vol
    v -= executed_vol;
    if (abs(volume - 0.0) < __DBL_EPSILON__)
        return 0; // current bar should be removed
    return 1;     // current bar still alive
}

void Bar::AddVolumesBy(double v)
{
    volume += v;
}
