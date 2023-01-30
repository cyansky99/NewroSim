#pragma once

class Material
{
protected:
    double minConductance;
    double maxConductance;

public:
    double RandomConductance();
    virtual double NewConductance(double conductance, int numPulse, int intensity) = 0;
};