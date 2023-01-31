#pragma once
#include "Cell.h"
#include "Wire.h"

class Array
{
private:
    const int X, Y;         // Array size
    Cell ***cell;           // Cells in an array
    Wire *wire;             // Wire property
    const double readNoise; // Read noise sigma

public:
    Array(int X, int Y, Wire *wire, Material *material, Transistor *transistor, double readNoise);
    Array(int X, int Y, Wire *wire, Material ***material, Transistor *transistor, double readNoise);
    void ReadArray(double *voltage, double *current);
    void ReadArrayBackwards(double *voltage, double *current);
    void WriteArray(int **numPulse);
    ~Array();
};