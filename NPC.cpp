#include "NPC.h"

//Emily Hull

//Constructor
NPC::NPC(const std::string& name, const std::string& type, int x, int y, int health):_name(name), _type(type), _x(x), _y(y), _health(health){}

NPC* NPC::spawn(const std::string& name, const std::string& type, int x, int y, int health){
    return new NPC(name, type, x, y, health);
}

//Allows me to see the NPC memory pool block.
std::span<std::byte> NPC::get_span(){
    return pool.get_memory_span();
}

//Allows the state of the object to be printed with cout.
std::ostream& operator<<(std::ostream& os, const NPC& obj){
    os << "NPC[name='" << obj._name << "', type='" << obj._type << "', pos=(" << obj._x << "," << obj._y << "), hp=" << obj._health << "]";
    return os;
}

//Get memory from my Pool class.
void* NPC::operator new(std::size_t size){
    return pool.allocate();
}

//Return memory to pool.
void NPC::operator delete(void* ptr) noexcept{
    pool.deallocate(ptr);
}