#include "Utils.hpp"

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
