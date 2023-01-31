#pragma once
#include "Array.h"

class Network
{
private:
    int layer;       // Number of layer
    Array **array;   // Hardware arrays consisting network
    int *dimension;  // Dimension of network
    double **output; // Output value of neuron
    double **error;  // Backpropagated error

public:
    Network(int layer, Array **array);
    ~Network();
};