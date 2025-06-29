#ifndef microhedger_utilities_bar_hpp
#define microhedger_utilities_bar_hpp

#include <vector>
#include <boost/format.hpp>

class Bar
{
private:
    double price;
    double volume;

public:
    Bar(double p, double v) : price(p), volume(v) {}
    Bar() : Bar(0.0, 0.0) {}
    ~Bar() {}

    inline double Price() const { return price; }
    inline double Volume() const { return volume; }
    inline bool IsEmptyBar() const { return abs(price) < __DBL_EPSILON__; }
    inline bool IsEmptyVolume() const { return abs(volume) < __DBL_EPSILON__; }
    inline bool IsEmpty() const { return IsEmptyBar() && IsEmptyVolume(); }

    int ExecuteAgainst(double &v);
    void AddVolumesBy(double v);
};

#endif