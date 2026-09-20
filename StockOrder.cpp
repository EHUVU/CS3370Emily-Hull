#include "StockOrder.h"

//Constructor
StockOrder::StockOrder(const std::string& symbol, const std::string& status, int quantity, double price) : _symbol(symbol), _status(status), _quantity(quantity), _price(price){};

//Factory method to ensure heap allocation.
StockOrder* StockOrder::spawn(const std::string& symbol, const std::string& status, int quantity, double price){
    return new StockOrder(symbol, status, quantity, price);
}

//Allows me to see the StockOrder pool block.
std::span<std::byte> StockOrder::get_span(){
    return pool.get_memory_span();
}

//Allows the state of the object to be printed with cout.
std::ostream& operator<<(std::ostream& os, const StockOrder& obj){
    os << "StockOrder[symbol='" << obj._symbol << "', status='" << obj._status << "', quantity=" << obj._quantity << ", price=" << obj._price << "]";
    return os;
}

//Get memory from my Pool class.
void* StockOrder::operator new(std::size_t size){
    return pool.allocate();
}

//Return memory to pool.
void StockOrder::operator delete(void* ptr) noexcept{
    pool.deallocate(ptr);
}