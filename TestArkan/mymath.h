#ifndef MYMATH_H
#define MYMATH_H
#include <iostream>
#include <math.h>
#include <complex>
#include <vector>


class MyMath
{
public:
    MyMath();
    static void Hypot(std::vector<std::complex<float>>* complexF, std::vector<float>* absF);
    static void Hypot(std::vector<std::complex<int32_t>>* complexF, std::vector<float>* absF);
};

#endif // MYMATH_H
