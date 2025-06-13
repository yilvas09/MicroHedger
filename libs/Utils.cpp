#include <cmath>
#include "Utils.hpp"

#define M_SQRT_2_PI sqrt(2 * M_PI)

void Utils::sortPairedVectors(std::vector<double> &A, std::vector<double> &B)
{
    std::vector<std::pair<double, double>> paired;
    paired.reserve(A.size());

    for (size_t i = 0; i < A.size(); ++i)
    {
        paired.emplace_back(A[i], B[i]);
    }

    // by default sort the paired vector based on the first element
    std::sort(paired.begin(), paired.end());

    // extract results
    for (size_t i = 0; i < paired.size(); ++i)
    {
        A[i] = paired[i].first;
        B[i] = paired[i].second;
    }
}

double Utils::normalCDF(double x)
{
    return 0.5 * erfc(-x * M_SQRT1_2);
}

double Utils::normalPDF(double x)
{
    return exp(-0.5 * x * x) / M_SQRT_2_PI;
}