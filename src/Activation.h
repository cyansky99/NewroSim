#pragma once

class Activation
{
protected:
    int outputBits;

public:
    int GetOuputBits();
    virtual int Activate(double input) = 0;
};