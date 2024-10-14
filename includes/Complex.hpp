#ifndef COMPLEX_HPP
#define COMPLEX_HPP

#include <iostream>

struct Complex
{
public:
    Complex();
    Complex(double realPart, double imaginaryPart);
    Complex(const Complex& other);
    Complex(const Complex&& other);
    ~Complex();
    
    Complex& operator=(const Complex& other);
    Complex& operator=(const Complex&& other);

    Complex operator*(const Complex& other);
    Complex operator+(const Complex& other);

    Complex& operator+=(const Complex& other);

    friend std::ostream& operator<<(std::ostream& os, const Complex& c);

public:
    double real;
    double img;
};

#endif