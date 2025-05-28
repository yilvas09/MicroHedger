#include <iostream>
#include "libs/LOB.hpp"
#include <vector>

int main()
{
    std::vector<double> ask_prices = {101.0, 102.0, 103.0};
    std::vector<double> ask_volumes = {100.0, 200.0, 150.0};
    std::vector<double> bid_prices = {99.0, 98.0, 97.0};
    std::vector<double> bid_volumes = {150.0, 100.0, 200.0};
    LOB currentLOB(ask_prices, ask_volumes, bid_prices, bid_volumes);
    currentLOB.PrintLOB();
    std::cout << "===================================" << std::endl;

    std::cout << "This LOB contains orders at price 1.1? " << currentLOB.ContainsPrice(1.1) << std::endl;
    std::cout << "This LOB contains orders at price 3.0? " << currentLOB.ContainsPrice(3.0) << std::endl;
    std::cout << "===================================" << std::endl;

    std::cout << "Location of sell order at price 3.0: " << currentLOB.PriceLocation(1, 3.0) << std::endl;
    std::cout << "Location of buy order at price 3.0: " << currentLOB.PriceLocation(-1, 3.0) << std::endl;
    std::cout << "Location of sell order at price 1.0: " << currentLOB.PriceLocation(1, 1.0) << std::endl;
    std::cout << "Location of buy order at price 1.0: " << currentLOB.PriceLocation(-1, 1.0) << std::endl;
    std::cout << "Location of buy order at price 1.1: " << currentLOB.PriceLocation(-1, 1.1) << std::endl;
    std::cout << "===================================" << std::endl;

    std::cout << "Adding a sell limit order of price 3.2 and volume 3:" << std::endl;
    currentLOB.AddLimitOrder(1, 3.2, 3);
    currentLOB.PrintLOB();
    std::cout << "===================================" << std::endl;

    std::cout << "Adding a sell limit order of price 3.2 and volume 2:" << std::endl;
    currentLOB.AddLimitOrder(1, 3.2, 3);
    currentLOB.PrintLOB();
    std::cout << "===================================" << std::endl;

    std::vector<Bar> executed_orders;
    double v_mo = 3;
    std::cout << "A sell market order of volume " << v_mo << " arrives:" << std::endl;
    double vwap = currentLOB.AbsorbMarketOrder(executed_orders, v_mo, 1);
    currentLOB.PrintLOB();
    std::cout << "Volumes of the not executed market order: " << v_mo << std::endl;
    std::cout << "Volume-weighted-average price of the executed market order: " << vwap << std::endl;
    std::cout << "===================================" << std::endl;

    v_mo = 10;
    std::cout << "A buy market order of volume " << v_mo << " arrives:" << std::endl;
    vwap = currentLOB.AbsorbMarketOrder(executed_orders, v_mo, -1);
    currentLOB.PrintLOB();
    std::cout << "Volumes of the not executed market order: " << v_mo << std::endl;
    std::cout << "Volume-weighted-average price of the executed market order: " << vwap << std::endl;
    std::cout << "Market failure? " << currentLOB.oneSideEmpty() << std::endl;
    std::cout << "===================================" << std::endl;

    return 0;
}