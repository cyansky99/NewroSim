#pragma once
#include "Activation.h"

class ADCSigmoid : public Activation
{
private:
    int MSBdigits;      // Number of MSBs
    int LSBdigits;      // Number of LSBs
    int numIntervalMSB; // Number of intervals in nonlinear ADC
    int numIntervalLSB; // Number of intervals in linear ADC
    double *lookup;     // Lookup table of nonlinear ADC

public:
    ADCSigmoid(int MSBdigits, int LSBdigits);
    int Activate(double input) override;
    double Derivative(int output) override;
    double GetMaxDiff() override;
    bool CanBeNegative() override;
    ~ADCSigmoid();
};