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
    Cell(int x, int y, Transistor *transistor, double conductance);
    void WriteCell(int numPulse);
    double ReadCell(double voltage, double wireResistivity, double readNoiseSigma);
    void PrintCell(double scale); // TODO: delete after debugging
};