#include <vector>
#include "PathCollection.hpp"

Path::Path(double hedger_pos, double hedger_iv)
    : hedger(hedger_pos, hedger_iv)
{
}

Path::~Path()
{
}

PathCollection::PathCollection(int n, const RandomInfo &ri)
    : n_paths(n), ran_info(ri), snapshots(std::vector<Path>(n_paths))
{
}

PathCollection::~PathCollection()
{
}

// this function should updates std::vector<Path> snapshots
void PathCollection::GeneratePaths()
{
}

void PathCollection::CalcLiquidityMetrics(std::vector<double>& res)
{

}