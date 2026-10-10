#pragma once

//Emily Hull

#include <compare>
#include <cstddef>
#include <iosfwd>
#include <string>
#include <vector>
#include <cstddef>

class BigInt {
public:
    // --- construction and conversion ---------------------------------
    BigInt();                                // the value 0
    BigInt(long long n);                     // implicit, on purpose
    explicit BigInt(const std::string& s);   // "42", "-17", "007"
    explicit operator bool() const;          // true if nonzero

    // --- compound assignment: the real arithmetic lives here ---------
    BigInt& operator+=(const BigInt& rhs);
    BigInt& operator-=(const BigInt& rhs);
    BigInt& operator*=(const BigInt& rhs);

    // --- unary, increment, decrement ---------------------------------
    BigInt  operator-() const;
    BigInt& operator++();       BigInt operator++(int);
    BigInt& operator--();       BigInt operator--(int);

    // --- inspection ---------------------------------------------------
    int operator[](std::size_t i) const;     // i-th decimal digit from the right
    std::size_t digitCount() const;          // 0 has one digit
    std::size_t digitCap() const;
    bool isNegative() const;
    std::string toString() const;

    // --- comparison ---------------------------------------------------
    friend bool operator==(const BigInt& a, const BigInt& b);
    friend std::strong_ordering operator<=>(const BigInt& a, const BigInt& b);

private:
    // your representation goes here
    std::vector<int> digits;
    void subtractHelper(const BigInt& rhs);
    int compareHelper(const BigInt& rhs) const;
    bool negative;
};

// --- non-member binary operators, built on the compound ones ---------
BigInt operator+(BigInt lhs, const BigInt& rhs);
BigInt operator-(BigInt lhs, const BigInt& rhs);
BigInt operator*(BigInt lhs, const BigInt& rhs);
std::ostream& operator<<(std::ostream& os, const BigInt& b);
