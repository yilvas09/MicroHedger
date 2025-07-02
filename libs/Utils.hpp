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

enum Parameter
{
    // path info
    N_DAYS = 0,
    N_HOURS = 1,
    N_QUARTERS = 2,
    INI_FUNDAMENTAL = 3,
    INI_LOB_VOLUME = 4,
    INI_LOB_DECAY = 5,
    HEDGER_OPTION_POSITION = 6,
    HEDGER_IMPLIED_VOLATILITY = 7,
    // random info
    RANDOM_SEED = 8,
    VOLATILITY_FUNDAMENTAL = 9,
    ORDER_INTENSITY = 10,
    PROB_MARKETORDER = 11,
    PROB_INFORMED = 12,
    PROB_BUY = 13,
    VOLUME_MIN = 14,
    VOLUME_MAX = 15,
    SPREAD_MEAN = 16,
    SPREAD_VOLATILITY = 17
};

#endif