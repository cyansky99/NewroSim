#pragma once
#include "Activation.h"

class IdealSigmoid : public Activation
{
private:
    double numInterval; // Number of intervals

public:
    IdealSigmoid(int outputBits);
    int Activate(double input) override;
    double Derivative(int output) override;
    double GetMaxDiff() override;
    bool CanBeNegative() override;
};