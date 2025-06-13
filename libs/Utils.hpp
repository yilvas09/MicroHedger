#ifndef microhedger_utilities_general_hpp
#define microhedger_utilities_general_hpp

#include <iostream>
#include <vector>

class Utils
{
public:
    Utils() {}
    ~Utils() {}

    static void sortPairedVectors(std::vector<double> &A, std::vector<double> &B);
    static double normalCDF(double x);
    static double normalPDF(double x);
    inline static int sgn(double x) { return (x > 0) - (x < 0); }
};

enum OrderType
{
    LIMITORDER = 0,
    MARKETORDER = 1
};

enum OptionType
{
    CALL = 0,
    PUT = 1,
    STRADDLE = 2,
    INVALID = 999
};

#endif