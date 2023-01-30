#pragma once
#include "Material.h"

class Cell
{
private:
    int x, y;
    Material *material;
    double conductance;

public:
    Cell(int x, int y, Material *material);
    void WriteCell(int numPulse, int intensity);
    double ReadCell(double voltage, double wireResistivity, double accessResistance, double readNoiseSigma);
};