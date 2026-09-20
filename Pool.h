#pragma once

// Emily Hull

#include <iostream>
#include <vector>
#include <cstddef>
#include <string>
#include <format>
#include <span>
#include <memory>
#include <new>
#include <typeinfo>

// class NPC;

template <typename T>
// Pool class handles custom memory management for NPC objects.
class Pool
{
private:
  std::byte *_memory_block;
  std::size_t _block_size;
  std::size_t _capacity;
  std::vector<std::size_t> free_indices;
  std::size_t live_objects = 0;
  bool trace_enabled;

  [[nodiscard]] void *get_slot(std::size_t index);

public:
  Pool(std::size_t count, bool trace = true);
  ~Pool();

  [[nodiscard]] void *allocate();
  [[nodiscard]] std::span<std::byte> get_memory_span();

  void deallocate(void *ptr);
  void profile() const;
};

/*
Changed this to handle template types. Text changed to Objects instead of just Npcs because it can handle more than just one type
*/
// Constructor
template <typename T>
Pool<T>::Pool(std::size_t count, bool trace) : _capacity(count)
{
  constexpr std::size_t align = alignof(T);
  _block_size = (sizeof(T) + align - 1) & ~(align - 1);
  _memory_block = static_cast<std::byte *>(::operator new[](count * _block_size, std::align_val_t(align)));
  trace_enabled = trace;
  for (std::size_t i = 0; i < count; i++)
  {
    free_indices.push_back(i);
  }

  if(trace_enabled){
    std::cout << "Initializing Object Pool with " << count << " slots (NPC size: (" << sizeof(T) << " bytes)" << std::endl;
  }
}

// Destructor
template <typename T>
Pool<T>::~Pool()
{
  constexpr std::size_t align = alignof(T);
  ::operator delete[](_memory_block, std::align_val_t(align));
}

// Returns a void pointer of the memory block, offset by the index times the block size.
template <typename T>
[[nodiscard]] void *Pool<T>::get_slot(std::size_t index)
{
  return _memory_block + index * _block_size;
}

// Pops an index from free_indices and returns the corresponding memory address fron get_slot() to be stored in ptr. Increments the live_objects value.
template <typename T>
[[nodiscard]] void *Pool<T>::allocate()
{
  if (free_indices.empty())
  {
    throw std::bad_alloc{};
  }
  std::size_t index = free_indices.back();
  free_indices.pop_back();

  void *ptr = get_slot(index);

  if (trace_enabled)
  {
    std::cout << "Allocating Object at slot " << index << "(address: " << ptr << ")" << std::endl;
  }

  live_objects++;
  return ptr;
}

// Calculates the index from the pointer, pushes back a slot into free_indices, prints a trace message if tracing is enabled and decrements the value of live_objects.
template <typename T>
void Pool<T>::deallocate(void *ptr)
{
  std::size_t index = (static_cast<std::byte *>(ptr) - _memory_block) / _block_size;

  free_indices.push_back(index);

  if (trace_enabled)
  {
    std::cout << "Deallocating Object at slot " << index << "(address: " << ptr << ")" << std::endl;
  }

  live_objects--;
}

// Prints out a status message of the available slots and the current amount of live npcs in the memory pool.
template <typename T>
void Pool<T>::profile() const
{
  std::cout << "Live Objects " << live_objects << ", Free Slots: " << free_indices.size() << std::endl;
  std::cout << "Free Slots: ";

  for (std::size_t i = free_indices.size() - 1; i > 0; --i)
  {
    std::size_t index = free_indices[i - 1];
    if (i > 1)
    {
      std::cout << index << ", ";
    }
    else
    {
      std::cout << index;
    }
  }

  std::cout << std::endl;
}

//Returns a view of the raw memory block.
template <typename T>
[[nodiscard]] std::span<std::byte> Pool<T>::get_memory_span()
{
  return std::span<std::byte>(_memory_block, _capacity * _block_size);
}