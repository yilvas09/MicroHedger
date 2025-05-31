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
};

#endif