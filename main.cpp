#include <iostream>
#include <vector>
#include <cstddef>
#include <string>
#include <format>
#include <span>
#include <memory>
#include "NPC.h"
#include "Pool.h"
#include "StockOrder.h"

/*
    I believe the professor mentioned that the test harness was optional for this assignment, so I did not include this.
    I edited the main.cpp file to include my other class for the memory pool test "StockOrder". This assignment wasn't too bad except I needed some time to remember
    how to do template classes. I also needed to read a bit about the alignof(T) since I never used this before. This assignment took a bit longer for me to complete than the previous one.
*/

//Main test class for Pool and NPC functionality.
int main(){

    //Test for NPC class.

    std::cout << "--------------NPC MEMORY TEST--------------" << std::endl;
    std::vector<NPC*> npcs;

    for(int i = 0; i < 10; ++i){
        npcs.push_back(NPC::spawn(
           "Goblin_" + std::to_string(i), "enemy", i * 10, i * 10, 50 + i * 5 
        ));
    }

    std::cout << *npcs[5] << std::endl;

    delete npcs[3];
    delete npcs[5];
    npcs[3] = nullptr;
    npcs[5] = nullptr;

    NPC::profile();

    npcs[3] = NPC::spawn("Dragon_Boss", "boss", 50, 50, 500);
    npcs[5] = NPC::spawn("Shopkeeper", "merchant", 25, 25, 100);

    for (auto* npc:npcs){
        delete npc;
    }

    NPC::profile();

    std::span<std::byte> npc_pool_span = NPC::get_span();
    std::cout << "Pool Memory: " << npc_pool_span.data() << std::endl;
    std::cout << "Pool Memory in bytes: " << npc_pool_span.size_bytes() << std::endl;

    std::cout << "\n--------------STOCKORDER MEMORY TEST--------------" << std::endl;
    std::vector<StockOrder*> stocks;

    for(int i = 0; i < 6; ++i){
        stocks.push_back(StockOrder::spawn(
           "NVDA", "ACCEPTED", 2, 222.07
        ));
    }

    std::cout << *stocks[5] << std::endl;

    delete stocks[1];
    delete stocks[2];
    stocks[1] = nullptr;
    stocks[2] = nullptr;

    StockOrder::profile();

    stocks[1] = StockOrder::spawn("GOOG", "PENDING", 1, 344.27);
    stocks[2] = StockOrder::spawn("TSLA", "ACCEPTED", 1, 364.23);

    for (auto* stock:stocks){
        delete stock;
    }

    StockOrder::profile();

    std::span<std::byte> stock_pool_span = StockOrder::get_span();
    std::cout << "Pool Memory: " << stock_pool_span.data() << std::endl;
    std::cout << "Pool Memory in bytes: " << stock_pool_span.size_bytes() << std::endl;


    return 0;
}