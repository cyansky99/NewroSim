#include <cmath>
#include "IdealSigmoid.h"

IdealSigmoid::IdealSigmoid(int outputBits)
{
    this->outputBits = outputBits;
    numInterval = pow(2, outputBits);
}

int IdealSigmoid::Activate(double input)
{
    double value = 1 / (1 + exp(-input));
    return static_cast<int>(value * numInterval);
}

double IdealSigmoid::Derivative(int output)
{
    double normalizedValue = static_cast<double>(output) / numInterval;
    return normalizedValue * (1 - normalizedValue);
}

double IdealSigmoid::GetMaxDiff()
{
    return 1.0 / 4;
}

bool IdealSigmoid::CanBeNegative()
{
    return false;
}