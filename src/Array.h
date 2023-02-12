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
    Cell **refCol;          // Reference column cells
    Cell **refRow;          // Reference row cells
    double maxWeight;       // Maximum weight ( |weight| < maxWeight x ItoV )
    double wireResistance;  // Wire unit resistance

public:
    Array(int X, int Y, Wire *wire, Material *material, Transistor *transistor, double readNoise);
    Array(int X, int Y, Wire *wire, Material ***material, Transistor *transistor, double readNoise);
    int GetX();
    int GetY();
    void ReadArray(double *voltage, double *current);
    void ReadArrayBackwards(double *voltage, double *current);
    double ReferenceColumn(double *voltage);
    double ReferenceRow(double *voltage);
    void WriteArray(int **numPulse);
    void IdealWriteArray(double **deltaConductance);
    double GetMaxWeight();
    void PrintArray(double scale);
    ~Array();
};