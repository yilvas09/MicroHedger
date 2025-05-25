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
    inline double Price() { return price; }
    inline double Volume() { return volume; }
    int ExecuteAgainst(double &v);
    void AddVolumesBy(double v);
    ~Bar() {}
};

#endif