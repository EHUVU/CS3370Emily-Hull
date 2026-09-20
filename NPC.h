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

//NPC class for testing Pool memory.
class NPC{
    private:
        std::string _name;
        std::string _type;

        int _x, _y;
        int _health;

        inline static Pool<NPC> pool{100, true}; //100 NPCS with tracing enabled.

        NPC(const std::string& name, const std::string& type, int x, int y, int health = 100);

    public:
        static NPC* spawn(const std::string& name, const std::string& type, int x, int y, int health);
        static std::span<std::byte> get_span();
        friend std::ostream& operator<<(std::ostream& os, const NPC& obj);
        static void* operator new(std::size_t size);
        static void operator delete(void* ptr) noexcept;
        static void profile() {pool.profile();}
};