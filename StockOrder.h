#pragma once

//Emily Hull

#include <iostream>
#include <vector>
#include <cstddef>
#include <string>
#include <format>
#include <span>
#include <memory>
#include "Pool.h"

//Test class for the Pool template.
class StockOrder{
    private:
        std::string _symbol;
        std::string _status;

        int _quantity;
        double _price;

        inline static Pool<StockOrder> pool{100, true}; //100 Stock Order objects with tracing enabled.

        StockOrder(const std::string& symbol, const std::string& status, int quantity, double price);

    public:
        static StockOrder* spawn(const std::string& symbol, const std::string& status, int quantity, double price);

        static std::span<std::byte> get_span();

        friend std::ostream& operator<<(std::ostream& os, const StockOrder& obj);
        static void* operator new(std::size_t size);
        static void operator delete(void* ptr) noexcept;
        static void profile() {pool.profile();}
};