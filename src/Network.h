#pragma once
#include "Array.h"
#include "Activation.h"

class Network
{
private:
    int layer;              // Number of layer
    Array **array;          // Hardware arrays consisting network
    Activation *activation; // Activation function
    int *dimension;         // Dimension of network
    int **output;           // Output value of neuron
    double **error;         // Backpropagated error
    double ItoV;            // Output current to voltage ratio (read pulse width / capacitance)
    double readVoltage;     // Voltage of read pulse
    int numBits;            // Number of bits in digital domain

public:
    Network(int layer, Array **array, Activation *activation, double ItoV, double readVoltage);
    void FF(double *input);
    void BP(int label);
    void WeightUpdate(double *learningRate, int streamLength, int numLevelLTP, int numLevelLTD);
    bool Test(int label);
    ~Network();
};