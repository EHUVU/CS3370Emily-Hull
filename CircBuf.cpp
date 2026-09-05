//Emily Hull

#include "CircBuf.h"

//Constructor. If the reserve does not equal zero, Have reserve adjust its size based on the CHUNK plus the current reserve - 1. Divide by CHUNK and then multiply the result by CHUNK.
CircBuf::CircBuf(size_t reserve){
    if(reserve != 0){
        reserve = ((reserve + CHUNK - 1) / CHUNK) * CHUNK;
    }
    buffer = new char[reserve]; //owning reference
	writehead = 0;
	readhead = 0;
	currSize = 0;
	currCapacity = reserve;

};

//Destructor
CircBuf::~CircBuf(){
	delete[] buffer;
};

//Get current size.
size_t CircBuf::size(){
    return currSize;
};

//Get current capacity.
size_t CircBuf::capacity(){
    return currCapacity;
};

//Helper function for insert.
void CircBuf::fill(){
    currCapacity = CHUNK;

    delete[] buffer;
    buffer = new char[currCapacity];

    for(size_t i = 0; i < currCapacity; i++){
        buffer[i] = '-';
    }
}

//Helper function for insert. Insert was getting too large and I wanted to break it up in smaller jobs to make it easier to read.
void CircBuf::resize(){
    size_t oldSize = currSize;
    size_t newCapacity = currCapacity + CHUNK;
	char* temp = new char[newCapacity]; //Create a temp buffer with the new capacity.
    for(size_t i = 0; i < newCapacity; i++){
        temp[i] = '-';
    }
	for(size_t i = 0; i < oldSize; i++){ //iterate only the size count because I am just moving numbers over.
		temp[i] = buffer[(readhead + i) % currCapacity];//temp[i] = this->get(); //start both buffers at the same position where readhead is.
	}

    delete[] buffer; //delete the old buffer.
	buffer = temp; //point the buffer at the new postion of the new address.

    currSize = oldSize;
    writehead = currSize;
    currCapacity = newCapacity;
    readhead = 0;
};

//Insert one character. Fill if the currCapacity is 0 and resize if the currentSize is larger or equal to the currCapacity.
void CircBuf::insert(char value){    
    if(currCapacity == 0){
        fill();
    }
	if(currSize >= currCapacity){
        resize();
	}
    buffer[writehead] = value;
	writehead++;

	if(writehead > currCapacity - 1){
		writehead = 0;
	}

    currSize++;
};

//Insert a dynamic character array.
void CircBuf::insert (const char* mchar, size_t sz){
    for(size_t i = 0; i < sz; i++){
        insert(mchar[i]);
    }
};

//Insert a string.
void CircBuf::insert(const string& str){
    for(char ch : str){
        insert(ch);
    }
};

//Get character at current readhead position and reduce the size by 1.
char CircBuf::get(){
    if(currSize == 0){
        return '\0';
    }

    char ch = buffer[readhead];
    buffer[readhead] = '-';

    readhead++;

    currSize--;

	return ch;
};

//Get a segment of size_t count.
string CircBuf::get(size_t count){
    string str;
    if(count > currSize){
        count = currSize;
    }
    for(size_t i = 0; i < count; i++){
        str.append(1, get());
    }

    return str;
};

//Output everything in the buffer and reduce the size of the buffer.
string CircBuf::flush(){
    string str;
    for(size_t i = 0; i < currSize; i++){
        str.append(1, buffer[(readhead + i) % currCapacity]);
    }

    delete[] buffer;
    buffer = nullptr;

	currCapacity = 0;
	currSize = 0;
    readhead = 0;
    writehead = 0;
	
	return str;
};

//Examine all of the contents of the array without changing it.
string CircBuf::examine(){
    string str;

    str.append("[");
    for(size_t i = 0; i < currCapacity; i++){
        str.append(1, buffer[i]);
    }
    str.append("]");
    return str;
};

//Shrink if the capacity is larger than the actual size.
void CircBuf::shrink(){
    if(currSize == 0 && currCapacity == CHUNK){
        delete[] buffer;
        buffer = nullptr;
        currCapacity = 0;
        readhead = 0;
        writehead = 0;
        return;
    }

    if(currCapacity > currSize && currCapacity - CHUNK >= currSize){
        size_t newCapacity = ((currSize + CHUNK - 1) / CHUNK) * CHUNK;

        char* temp = new char[newCapacity];

        for(size_t i = 0; i < newCapacity; i++){
            temp[i] = '-';
        }

        for(size_t i = 0; i < currSize; i++){
		    temp[i] = buffer[(readhead + i) % currCapacity];
	    }

        delete[] buffer;
        buffer = temp;

        currCapacity = newCapacity;

        readhead = 0;

        if(currCapacity == 0){
            writehead = 0;
        }else{
            writehead = currSize % currCapacity;
        }
    }
};