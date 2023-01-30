#pragma once
#include "Cell.h"

class Array
{
private:
    int X, Y;
    Cell ***cell;
    double wireResistivity;
    double readNoise;

public:
    Array(int X, int Y, Material *material, double wireResistivity, double readNoise);
    Array(int X, int Y, Material ***material, double wireResistivity, double readNoise);
    double *ReadArray(double *voltage);
    ~Array();
};