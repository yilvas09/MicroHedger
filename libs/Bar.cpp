#include "Bar.hpp"

double Bar::tick_size = __DBL_EPSILON__;

Bar::Bar(double p, double v)
    : volume(v)
{
    price = tick_size <= __DBL_EPSILON__ ? p : std::round(p / tick_size) * tick_size;
}

Bar::Bar()
    : Bar(0.0, 0.0)
{
}

bool Bar::PriceSameAs(double p) const
{
    return abs(price - p) < tick_size / 2;
}

bool Bar::PriceHigherThan(double p) const
{
    return p > price + tick_size / 2;
}

bool Bar::PriceLowerThan(double p) const
{
    return p < price - tick_size / 2;
}

void Bar::SetTickSize(double ts)
{
    if (tick_size > __DBL_EPSILON__)
        throw std::logic_error("Tick size cannot be set again as it has already been set to non zero value.");
    if (ts < __DBL_EPSILON__)
        throw std::invalid_argument("Tick size must be non-zero positive number.");
    tick_size = ts;
}

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
