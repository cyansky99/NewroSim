#include "Transistor.h"

Transistor::Transistor(double onResistance) : onResistance(onResistance)
{
}

double Transistor::getOnResistance()
{
    return onResistance;
}