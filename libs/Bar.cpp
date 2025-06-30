#include "Bar.hpp"

const double EPSILON = 1e-9;
const double MIN_TICKSIZE = 2 * __DBL_EPSILON__;
double Bar::tick_size = MIN_TICKSIZE;

Bar::Bar(double p, double v)
    : volume(v)
{
    price = tick_size <= MIN_TICKSIZE ? p : std::round(p / tick_size + EPSILON) * tick_size;
}

Bar::Bar()
    : Bar(0.0, 0.0)
{
}

bool Bar::PriceSameAs(double p) const
{
    Bar newBar(p, 0.0);
    return abs(price - newBar.price) < __DBL_EPSILON__;
}

bool Bar::PriceHigherThan(double p) const
{
    Bar newBar(p, 0.0);
    return price > newBar.price - __DBL_EPSILON__;
}

bool Bar::PriceLowerThan(double p) const
{
    Bar newBar(p, 0.0);
    return price < newBar.price + __DBL_EPSILON__;
}

bool Bar::PriceHigherEqual(double p) const
{
    return PriceHigherThan(p) || PriceSameAs(p);
}

bool Bar::PriceLowerEqual(double p) const
{
    return PriceLowerThan(p) || PriceSameAs(p);
}

void Bar::SetTickSize(double ts)
{
    if (tick_size > MIN_TICKSIZE)
        throw std::logic_error("Tick size cannot be set again as it has already been set to non zero value.");
    if (ts < MIN_TICKSIZE)
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
