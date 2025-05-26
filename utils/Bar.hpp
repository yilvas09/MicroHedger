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
    Bar() : price(0.0), volume(0.0) {}
    Bar(double p, double v) : price(p), volume(v) {}
    ~Bar() {}

    inline double Price() const { return price; }
    inline double Volume() const { return volume; }

    int ExecuteAgainst(double &v);
    void AddVolumesBy(double v);
};

#endif