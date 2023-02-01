#include "Cell.h"
#include <random>

extern std::mt19937 gen;

Cell::Cell(int x, int y, Material *material, Transistor *transistor)
    : x(x), y(y), material(material), transistor(transistor)
{
    conductance = material->RandomConductance();
}

Cell::Cell(int x, int y, Transistor *transistor, double conductance)
    : x(x), y(y), transistor(transistor), conductance(conductance)
{
}

void Cell::WriteCell(int numPulse)
{
    conductance = material->NewConductance(conductance, numPulse);
}

double Cell::ReadCell(double voltage, double wireResistance, double readNoiseSigma)
{
    double current = voltage / (1 / conductance + wireResistance * (x + y + 2) + transistor->getOnResistance());
    if (readNoiseSigma)
    {
        std::uniform_real_distribution<double> dis(0, readNoiseSigma);
        return current * (1 + dis(gen));
    }
    else
        return current;
};