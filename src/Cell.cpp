#include "Cell.h"
#include <random>

extern std::mt19937 gen;

Cell::Cell(int x, int y, Material *material, Transistor *transistor)
    : x(x), y(y), material(material), transistor(transistor)
{
    conductance = material->RandomConductance();
    accessResistance = transistor->getOnResistance();
}

Cell::Cell(int x, int y, Material *material, Transistor *transistor, double conductance)
    : x(x), y(y), material(material), transistor(transistor), conductance(conductance)
{
    accessResistance = transistor->getOnResistance();
}

void Cell::WriteCell(int numPulse)
{
    conductance = material->NewConductance(conductance, numPulse);
}

void Cell::IdealWriteCell(double deltaConductance)
{
    double newConductance = conductance + deltaConductance;
    if (newConductance > material->MaxConductance())
        conductance = material->MaxConductance();
    else if (newConductance < material->MinConductance())
        conductance = material->MinConductance();
    else
        conductance = newConductance;
}

double Cell::ReadCell(double voltage, double wireResistance, double readNoiseSigma)
{
    double current = voltage / (1 / conductance + wireResistance * (x + y + 2) + accessResistance);
    if (readNoiseSigma)
    {
        std::uniform_real_distribution<double> dis(0, readNoiseSigma);
        return current * (1 + dis(gen));
    }
    else
        return current;
};

void Cell::PrintCell(double scale) // TODO: delete after debugging
{
    if (conductance > (material->MaxConductance() + material->MinConductance()) / 2)
        printf("+");
    printf("%.5lf", (conductance - (material->MaxConductance() + material->MinConductance()) / 2) * scale);
}