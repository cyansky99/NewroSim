#pragma once

class Wire
{
private:
    double width, length, height; // Wire Size (nm)
    double resistivity;           // Wire Resistivity (Ohm m)
    double unitResistance;        // Resistance per cell length
public:
    Wire(double width, int cellSize, double aspectRatio, double resistivity);
    double UnitResistance();
};