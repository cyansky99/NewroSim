#include "ADCSigmoid.h"
#include <cmath>

ADCSigmoid::ADCSigmoid(int MSBdigits, int LSBdigits)
    : MSBdigits(MSBdigits), LSBdigits(LSBdigits)
{
    /* Constructing lookup table of nonlinear ADC */
    outputBits = MSBdigits + LSBdigits;
    numIntervalMSB = pow(2, MSBdigits);
    numIntervalLSB = pow(2, LSBdigits);
    lookup = new double[numIntervalMSB];
    lookup[0] = -INFINITY;
    for (int i = 1; i < numIntervalMSB; i++)
    {
        double y = static_cast<double>(i) / numIntervalMSB;
        lookup[i] = log(y / (1 - y));
    }
}

int ADCSigmoid::Activate(double input)
{
    int MSB;
    int LSB;
    if (lookup[1] > input)
        return 0; // Return minimum
    for (int i = 2; i < numIntervalMSB; i++)
    {
        if (lookup[i] > input)
        {
            MSB = i - 1;
            LSB = static_cast<int>((input - lookup[i - 1]) / (lookup[i] - lookup[i - 1]) * numIntervalLSB);
            return MSB * numIntervalLSB + LSB;
        }
    }
    return numIntervalLSB * numIntervalMSB; // Return maximum
}

ADCSigmoid::~ADCSigmoid()
{
    delete[] lookup;
}