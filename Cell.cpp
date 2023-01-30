#include "Cell.h"
#include <random>

extern std::mt19937 gen;

Cell::Cell(int x, int y, Material *material)
    : x(x), y(y), material(material)
{
    conductance = material->RandomConductance();
}

void Cell::WriteCell(int numPulse, int intensity)
{
    conductance = material->NewConductance(conductance, numPulse, intensity);
}

double Cell::ReadCell(double voltage, double wireResistivity, double accessResistance, double readNoiseSigma)
{
    double current = voltage / (1 / conductance + wireResistivity * (x + y) + accessResistance);
    if (readNoiseSigma)
    {
        std::uniform_real_distribution<double> dis(0, readNoiseSigma);
        return current * (1 + dis(gen));
    }
    else
        return current;
};