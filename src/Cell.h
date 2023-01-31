#pragma once
#include "Material.h"
#include "Transistor.h"

class Cell
{
private:
    int x, y;               // Cell location
    Material *material;     // Memristor material in a cell
    Transistor *transistor; // Access transistor in a cell
    double conductance;     // Cell conductance

public:
    Cell(int x, int y, Material *material, Transistor *transistor);
    void WriteCell(int numPulse);
    double ReadCell(double voltage, double wireResistivity, double readNoiseSigma);
};