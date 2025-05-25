#include <iostream>
#include "utils/LOB.hpp"
#include <vector>

int main()
{
    std::vector<double> askprices = {4.1, 4.5, 5.2};
    std::vector<double> askvolume = {2, 4, 1};
    std::vector<double> bidprices = {1.1, 1.5, 3.2};
    std::vector<double> bidvolume = {1, 4, 2};
    LOB currentLOB(askprices, askvolume, bidprices, bidvolume);
    currentLOB.PrintLOB();
    std::cout<<"==================================="<<std::endl;

    std::cout<<"This LOB contains orders at price 1.1? "<<currentLOB.ContainsPrice(1.1)<<std::endl;
    std::cout<<"This LOB contains orders at price 3.0? "<<currentLOB.ContainsPrice(3.0)<<std::endl;
    std::cout<<"==================================="<<std::endl;

    std::cout<<"Location of sell order at price 3.0: "<<currentLOB.PriceLocation(1, 3.0)<<std::endl;
    std::cout<<"Location of buy order at price 3.0: "<<currentLOB.PriceLocation(-1, 3.0)<<std::endl;
    std::cout<<"Location of sell order at price 1.0: "<<currentLOB.PriceLocation(1, 1.0)<<std::endl;
    std::cout<<"Location of buy order at price 1.0: "<<currentLOB.PriceLocation(-1, 1.0)<<std::endl;
    std::cout<<"Location of buy order at price 1.0: "<<currentLOB.PriceLocation(-1, 1.1)<<std::endl;
    std::cout<<"==================================="<<std::endl;

    currentLOB.AddLimitOrder(1, 3.2, 3);
    currentLOB.PrintLOB();

    return 0;
}