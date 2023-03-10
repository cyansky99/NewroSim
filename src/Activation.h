#pragma once

class Activation
{
protected:
    int outputBits; // Number of output bits

public:
    int GetOuputBits();
    virtual int Activate(double input) = 0;    // Get output of activation function
    virtual double Derivative(int output) = 0; // Derivative of activation function
    virtual double GetMaxDiff() = 0;           // Get maximum differential coefficient of activation function
    virtual bool CanBeNegative() = 0;          // Can give negative outputs
    virtual int Maximum() = 0;                 // Maximum output value
    virtual int Minimum() = 0;                 // Minimum output value
};