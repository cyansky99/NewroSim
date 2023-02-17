#pragma once
#include "Activation.h"

class IdealTanh : public Activation
{
private:
    double numInterval; // Number of intervals

public:
    IdealTanh(int outputBits);
    int Activate(double input) override;
    double Derivative(int output) override;
    double GetMaxDiff() override;
    bool CanBeNegative() override;
    int Maximum() override;
    int Minimum() override;
};