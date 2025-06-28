#ifndef microhedger_utilities_bar_hpp
#define microhedger_utilities_bar_hpp

#include <vector>
#include <boost/format.hpp>

class Bar
{
private:
    double price;
    double volume;

    static double tick_size;

public:
    Bar(double p, double v);
    Bar();
    ~Bar() {}

    inline double Price() const { return price; }
    inline double Volume() const { return volume; }
    inline bool IsEmptyBar() const { return abs(price) < tick_size; }
    inline bool IsEmptyVolume() const { return abs(volume) < __DBL_EPSILON__; }
    inline bool IsEmpty() const { return IsEmptyBar() && IsEmptyVolume(); }

    bool PriceSameAs(double p) const;
    bool PriceHigherThan(double p) const;
    bool PriceLowerThan(double p) const;

    static void SetTickSize(double ts);

    int ExecuteAgainst(double &v);
    void AddVolumesBy(double v);
};

#endif