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
    int intValue = static_cast<int>(value * numInterval);
    if (value - intValue >= 0.5 && intValue < numInterval - 1)
        return intValue + 1;
    else
        return intValue;
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

int IdealSigmoid::Maximum()
{
    return numInterval - 1;
}

int IdealSigmoid::Minimum()
{
    return 0;
}