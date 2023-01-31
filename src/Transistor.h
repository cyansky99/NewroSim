#pragma once

class Transistor
{
private:
    double onResistance; // Resistance when ON

public:
    Transistor(double onResistance);
    double getOnResistance();
};