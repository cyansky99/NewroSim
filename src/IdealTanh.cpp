#include <cmath>
#include "IdealTanh.h"

IdealTanh::IdealTanh(int outputBits)
{
    this->outputBits = outputBits;
    numInterval = pow(2, outputBits);
}

int IdealTanh::Activate(double input)
{
    double value = tanh(input);
    int intValue = static_cast<int>(value * numInterval / 2);
    if (value > 0)
    {
        if (value - intValue >= 0.5 && intValue < numInterval - 1)
            return intValue + 1;
        else
            return intValue;
    }
    else
    {
        if (intValue - value > 0.5)
            return intValue - 1;
        else
            return intValue;
    }
}

double IdealTanh::Derivative(int output)
{
    double normalizedValue = static_cast<double>(output) / numInterval;
    return 1 - pow(normalizedValue, 2);
}

double IdealTanh::GetMaxDiff()
{
    return 1;
}

bool IdealTanh::CanBeNegative()
{
    return true;
}

int IdealTanh::Maximum()
{
    return numInterval / 2 - 1;
}

int IdealTanh::Minimum()
{
    return -numInterval / 2;
}