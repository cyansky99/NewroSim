#include "Wire.h"

Wire::Wire(double width, int cellSize, double aspectRatio, double resistivity) : width(width), resistivity(resistivity)
{
    length = width * cellSize;
    height = width * aspectRatio;
    unitResistance = resistivity * length / (width * height * 1e-9);
}
double Wire::UnitResistance()
{
    return unitResistance;
}