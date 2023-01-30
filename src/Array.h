#pragma once
#include "Cell.h"

class Array
{
private:
    int X, Y;
    Cell ***cell;
    double wireResistivity;

public:
    Array(int X, int Y, Material *material);
    Array(int X, int Y, Material ***material);
    ~Array();
};