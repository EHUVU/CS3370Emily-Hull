#include "bigint.h"
#include <iostream>
#include <regex>
#include <stdexcept>

/*
Default constructor - Pushes back 0 and sets negative to false;
*/
BigInt::BigInt()
{
    digits.push_back(0);
    negative = false;
}

/*
long long constructor - I am sure there is a better way to do this, but I figured I would turn the value into a string and
push it into the digits vector. This constructor also takes into account the negative sign that might be in the string.
*/
BigInt::BigInt(long long n)
{
    std::string str = std::to_string(n);
    digits.clear();
    if (n < 0)
    {
        negative = true;
    }
    else if (n >= 0)
    {
        negative = false;
    }
    for (int i = static_cast<int>(str.size()) - 1; i >= 0; --i)
    {
        if (str[i] == '-')
        {
            continue;
        }
        digits.push_back(static_cast<int>(str[i] - '0'));
    }

    while (digits.size() > 1 && digits.back() == 0)
    {
        digits.pop_back();
    }
    if (digits.empty())
    {
        digits.push_back(0);
    }

    if (digits.size() == 1 && digits[0] == 0)
    {
        negative = false;
    }
}

/*
String constructor - Matches it against a regex to ensure that characters that are not numbers cannot be passed into the BigInt digit vector.
*/
BigInt::BigInt(const std::string &s)
{
    if (!std::regex_match(s, std::regex("[+-]?[0-9]+")))
    {
        throw std::invalid_argument("Invalid BigInt");
    }

    digits.clear();

    if (negative = (!s.empty() && s[0] == '-'))
    {
        negative = true;
    }

    std::size_t firstValue = 0;

    if (!s.empty() && (s[0] == '-' || s[0] == '+'))
    {
        firstValue = 1;
    }

    for (std::size_t i = s.size(); i > firstValue; --i)
    {
        digits.push_back(s[i - 1] - '0');
    }

    while (digits.size() > 1 && digits.back() == 0)
    {
        digits.pop_back();
    }

    if (digits.empty())
    {
        digits.push_back(0);
    }

    if (digits.size() == 1 && digits[0] == 0)
    {
        negative = false;
    }
}

BigInt::operator bool() const
{
    if (digits.at(0) == 0)
    {
        return false;
    }
    return true;
}

// Addition operator for two BigInt objects.
BigInt &BigInt::operator+=(const BigInt &rhs)
{
    int carry = 0;
    int value = 0;
    std::size_t maxsize = std::max(digits.size(), rhs.digits.size());

    if (digits.size() < maxsize)
    {
        digits.resize(maxsize, 0);
    }

    for (std::size_t i = 0; i < maxsize; ++i)
    {
        value = digits[i] + carry;
        if (i < rhs.digits.size())
        {
            value += rhs.digits[i];
        }
        digits[i] = value % 10;
        carry = value / 10;
    }
    if (carry > 0)
    {
        digits.push_back(carry);
    }

    return *this;
}

//Subtraction operator for this and a rhs BigInt object.
BigInt &BigInt::operator-=(const BigInt &rhs)
{
    int borrow = 0;
    int value = 0;
    std::size_t maxsize = std::max(digits.size(), rhs.digits.size());

    if (digits.size() < maxsize)
    {
        digits.resize(maxsize, 0);
    }

    for (std::size_t i = 0; i < maxsize; ++i)
    {
        value = digits[i] - borrow;
        if (i < rhs.digits.size())
        {
            value -= rhs.digits[i];
        }
        if (value < 0)
        {
            value += 10;
            borrow = 1;
        }
        else
        {
            borrow = 0;
        }

        digits[i] = value;
    }

    if (borrow)
    {
        negative = true;

        for (std::size_t i = 0; i < digits.size(); ++i)
        {
            digits[i] = 9 - digits[i];
        }

        for (std::size_t i = 0; i < digits.size(); ++i)
        {
            ++digits[i];
            if (digits[i] < 10)
            {
                break;
            }
            digits[i] = 0;
        }
    }
    while (digits.size() > 1 && digits.back() == 0)
    {
        digits.pop_back();
    }

    if (digits.size() == 1 && digits[0] == 0)
    {
        negative = false;
    }

    return *this;
}

//Multiplication operator for this and a rhs BigInt object.
BigInt &BigInt::operator*=(const BigInt &rhs)
{
    bool resultNegative = (negative != rhs.negative);

    BigInt result;
    result.digits.assign(digits.size() + rhs.digits.size(), 0);

    for (std::size_t i = 0; i < digits.size(); ++i)
    {
        int carry = 0;

        for (std::size_t j = 0; j < rhs.digits.size(); ++j)
        {
            int value = result.digits[i + j] + digits[i] * rhs.digits[j] + carry;

            result.digits[i + j] = value % 10;
            carry = value / 10;
        }

        std::size_t pos = i + rhs.digits.size();

        while (carry > 0)
        {
            int value = result.digits[pos] + carry;
            result.digits[pos] = value % 10;
            carry = value / 10;
            ++pos;
        }
    }

    while (result.digits.size() > 1 &&
           result.digits.back() == 0)
    {
        result.digits.pop_back();
    }

    result.negative = resultNegative;

    if (result.digits.size() == 1 && result.digits[0] == 0)
    {
        result.negative = false;
    }

    *this = result;
    return *this;
}

//Unary operator that flips the value of negative.
BigInt BigInt::operator-() const
{
    BigInt result(*this);
    if (result.digits.size() == 1 && result.digits[0] == 0)
    {
        result.negative = false;
    }
    else
    {
        result.negative = !result.negative;
    }
    return result;
}

//Increment Bigint operator takes the current instance of BigInt and increments the value.
BigInt &BigInt::operator++()
{
    *this += BigInt(1);
    return *this;
}

//Increment Bigint operator takes the current instance of BigInt and increments the value post-increment.
BigInt BigInt::operator++(int)
{
    BigInt prev = *this;
    ++(*this);
    return prev;
}

//Decrement Bigint operator takes the current instance of BigInt and decrements the value.
BigInt &BigInt::operator--()
{
    *this -= BigInt(1);
    return *this;
}

//Decrement Bigint operator takes the current instance of BigInt and decrements the value post-decrement.
BigInt BigInt::operator--(int)
{
    BigInt prev = *this;
    --(*this);
    return prev;
}

//-------Inspection

//Checks the current value residing where i is located.
int BigInt::operator[](std::size_t i) const
{
    return digits.at(i);
}

//Returns the size of the digits vector.
std::size_t BigInt::digitCount() const
{
    return digits.size();
}

//Returns the value of negative.
bool BigInt::isNegative() const
{
    return negative;
}

//Returns the result as a string.
std::string BigInt::toString() const
{
    std::string str;

    if (digits.empty())
        return "0";

    bool isZero = true;
    for (int digit : digits)
    {
        if (digit != 0)
        {
            isZero = false;
            break;
        }
    }

    if (isZero)
        return "0";

    if (negative)
    {
        str.push_back('-');
    }

    for (std::size_t i = digits.size(); i > 0; --i)
    {
        str.push_back(static_cast<char>('0' + digits[i - 1]));
    }

    return str;
}

//--------Comparison

//Allows two bigints to be compared if equal. 
bool operator==(const BigInt &a, const BigInt &b)
{
    if (a.digits == b.digits && a.isNegative() == b.isNegative())
    {
        return true;
    }
    return false;
}

//Spaceship operator allows for greater than, less than and equal to comparisons for bigint objects.
std::strong_ordering operator<=>(const BigInt &a, const BigInt &b)
{
    if (a.digits.size() > b.digits.size())
    {
        return std::strong_ordering::greater;
    }
    if (a.digits.size() < b.digits.size())
    {
        return std::strong_ordering::less;
    }
    return std::strong_ordering::equal;
}

//--------Compound

/*
Allows the use of the addition operator for a lvalue and rvalue big int.
*/
BigInt operator+(BigInt lhs, const BigInt &rhs)
{
    return lhs.operator+=(rhs);
}

/*
Allows the use of the subtraction operator for a lvalue and rvalue big int.
*/
BigInt operator-(BigInt lhs, const BigInt &rhs)
{
    return lhs.operator-=(rhs);
}

/*
Allows the use of the multiplication operator for a lvalue and rvalue big int.
*/
BigInt operator*(BigInt lhs, const BigInt &rhs)
{
    return lhs.operator*=(rhs);
}

//Allows the bigint contents to be output through std::cout.
std::ostream &operator<<(std::ostream &os, const BigInt &b)
{
    if (b.isNegative())
    {
        os << '-';
    }
    return os << b.toString();
}