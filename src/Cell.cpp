#include "Cell.h"
#include <iostream>

Cell::Cell(int x, int y, Material *material)
    : x(x), y(y), material(material)
{
    conductance = material->RandomConductance();
}

void Cell::WriteCell(int numPulse, int intensity)
{
    conductance = material->NewConductance(conductance, numPulse, intensity);
}

void Cell::PrintCell()
{
    printf("%.20lf\n", conductance);
}