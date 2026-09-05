#pragma once

//Emily Hull
//Code supplied by the instructor.

#include <string>
#include <cstddef>
using std::string;

class CircBuf {
	const size_t CHUNK { 8 };
	// Insert your private stuff here
	char* buffer;
	size_t writehead;
	size_t readhead;
	size_t currSize;
	size_t currCapacity;
	void fill();
	void resize();

public:
	CircBuf(size_t reserve = 0);// Number of elements you want it to be able to hold to start with.
	~CircBuf();

	size_t size();
	size_t capacity();

	void insert(char value);
	void insert (const char* mchar, size_t sz);
	void insert(const string&);
	char get();
	string get(size_t);
	string flush();	// Returns a string with all the characters, AND shrinks the buffer to zero.
	string examine();
	void shrink();
};
