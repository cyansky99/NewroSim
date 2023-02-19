#pragma once
#include "Array.h"
#include "Activation.h"

class Network
{
private:
    int layer;                // Number of layer
    Array **array;            // Hardware arrays consisting network
    Activation *activation;   // Activation function
    int *dimension;           // Dimension of network
    int **output;             // Output value of neuron
    double **error;           // Backpropagated error
    const double ItoV;        // Output current to voltage ratio (read pulse width / capacitance)
    const double readVoltage; // Voltage of read pulse
    const int ADCsteps;       // Steps of ADC for backpropagation
    int numBits;              // Number of bits used in feedforward
    double *errorRange;       // errorRange[l] : Size range of error at layer l + 1

public:
    Network(int layer, Array **array, Activation *activation, double ItoV, double readVoltage, int ADCsteps);
    void FF(double *input);
    void BP(int label);
    void StochasticPulseWU(double *learningRate, int streamLength, int numLevelLTP, int numLevelLTD);
    unsigned long long int StochasticPulseWU(double *learningRate, int streamLength, int numLevelLTP, int numLevelLTD, int n); // Returns number of mismatched pulses
    void IdealWU(double *learningRate);
    void HardwareWU(double *learningRate, double conductanceRange, int numLevelLTP, int numLevelLTD);
    bool Test(int label);
    void SnapShot(int i);
    ~Network();
};