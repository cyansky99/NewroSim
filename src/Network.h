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

public:
    Network(int layer, Array **array, Activation *activation);
    void FF(double *input, double readVoltage, double ItoV);
    void BP();
    void WeightUpdate(double learningRate);
    ~Network();
};