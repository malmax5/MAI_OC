#include "../includes/Complex.hpp"

Complex::Complex() : real(0), img(0) {}
Complex::Complex(double realPart, double imaginaryPart) : real(realPart), img(imaginaryPart) {}
Complex::Complex(const Complex& other) : real(other.real), img(other.img) {}
Complex::Complex(const Complex&& other) 
    : real(std::move(other.real)), img(std::move(other.img)) {}
Complex::~Complex() {}

Complex& Complex::operator=(const Complex& other)
{
    if (this != &other)
    {
        this->real = other.real;
        this->img = other.img;
    }

    return *this;
}

Complex& Complex::operator=(const Complex&& other)
{
    if (this != &other)
    {
        this->real = std::move(other.real);
        this->img = std::move(other.img);
    }

    return *this;
}

Complex Complex::operator+(const Complex& other)
{
    Complex res;
    res.real = this->real + other.real;
    res.img = this->img + other.img;

    return res;
}

Complex& Complex::operator+=(const Complex& other)
{
    this->real += other.real;
    this->img += other.img;

    return *this;
}

Complex Complex::operator*(const Complex& other)
{
    Complex res;
    res.real = this->real * other.real - this->img * other.img;
    res.img = this->real * other.img + other.real * this->img;
    
    return res;
}

std::ostream& operator<<(std::ostream& os, const Complex& c)
{
    os << "(" << c.real << ", " << c.img << ")";
    return os;
}