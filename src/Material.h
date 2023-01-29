#pragma once

class Material
{
protected:
    const double minConductance;
    const double maxConductance;

public:
    Material(double minConductance, double maxConductance);
    double RandomConductance();
    virtual double NewConductance(double conductance, int numPulse, int intensity) = 0;
    ~Material();
};