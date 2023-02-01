#pragma once

class Material
{
protected:
    double minConductance; // Minimum conductance
    double maxConductance; // Maximum conductance

public:
    Material(double minConductance, double maxConductance);
    double RandomConductance();
    double MinConductance();
    double MaxConductance();
    virtual double NewConductance(double conductance, int numPulse) = 0;
};