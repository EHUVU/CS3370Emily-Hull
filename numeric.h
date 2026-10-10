#pragma once

#include <concepts>

template <typename T>
concept NumberLike = 
    requires(T a, T b){
        T{0};
        T{1};

        {a + b} -> std::same_as<T>;
        {a - b} -> std::same_as<T>;
        {a * b} -> std::same_as<T>;
        {a += b} -> std::same_as<T&>;
        {a -= b} -> std::same_as<T&>;
        {a *= b} -> std::same_as<T&>;
        {a == b} -> std::convertible_to<bool>;
    };

template <NumberLike T>
T power(T base, unsigned exponent){
    T result{1};

    while (exponent > 0){
        if(exponent % 2 == 1){
            result *= base;
        }
        base *= base;
        exponent /= 2;
    }
    return result;
}

template <NumberLike T>
T factorial(unsigned n){
    T result{1};
    for(unsigned i = 2; i <= n; ++i){
        result *= T{i};
    }
    return result;
}

template <NumberLike T>
T fibonacci(unsigned n){
    if(n == 0){
        return T{1};
    }

    T a{1};
    T b{1};

    for(unsigned i = 3; i <= n; ++i){
        T next = a + b;
        a = b;
        b = next;
    }
    return b;
}
