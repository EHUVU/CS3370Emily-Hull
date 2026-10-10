//Emily Hull

#include "test_bigint.h"
#include "perf_bigint.h"
#include <chrono>
#include <iomanip>

/*
My main function with the byte, capacity and number of elements.

Admittedly, this assignment was a bit difficult and very time consuming. I wasn't sure if I needed to do operator /= or %=? It
looks like these are bonus functions? The assignment pdf isn't super clear on this and the structure of bigint.g does not contain
these.

COMPILER OPTIMIZATION RESULTS:
No Optimization:

Summary
  best time:               10755.92 ms
  heap held by the result: 142640 bytes  (4.000 bytes per decimal digit)

Level 2:

Summary
  best time:               1309.99 ms
  heap held by the result: 142640 bytes  (4.000 bytes per decimal digit)

  This is an insane difference!

  The reason I chose ints for my representation is because it was simple to use and cut into individual numbers for my digit vector.
*/
int main(){
    runRequiredChecks();
    runPerformanceReport();

    auto start = std::chrono::steady_clock::now();
    
    BigInt huge = factorial<BigInt>(10000);

    auto end = std::chrono::steady_clock::now();
    
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Representation: int" << std::endl;
    std::cout << "Elements: " << huge.digitCount() << std::endl;
    std::cout << "Capacity: "<< huge.digitCap() <<std::endl;
    std::cout << "Bytes: "<< huge.digitCap() * sizeof(int) <<std::endl;

    std::cout << elapsed.count() << std::endl;

    return 0;
}