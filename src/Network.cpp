#include "Network.h"
#include <iostream>
#include <cmath>

Network::Network(int layer, Array **array, Activation *activation, double ItoV, double readVoltage)
    : layer(layer), array(array), activation(activation), ItoV(ItoV), readVoltage(readVoltage)
{
    /* Checking if the sizes of arrays are compatible */
    for (int i = 0; i < layer - 2; i++)
    {
        if (array[i]->GetY() != array[i + 1]->GetX())
        {
            std::cout << "Layer sizes do not match" << std::endl;
            exit(-1);
        }
    }

    /* Checking network dimension */
    dimension = new int[layer];
    for (int i = 0; i < layer - 1; i++)
        dimension[i] = array[i]->GetX();
    dimension[layer - 1] = array[layer - 2]->GetY();

    /* Number of bits used in digital domain */
    numBits = activation->GetOuputBits();

    /* Mermory allocation for members */
    output = new int *[layer];
    for (int i = 0; i < layer; i++)
    {
        output[i] = new int[dimension[i]];
    }
    error = new double *[layer - 1];
    for (int i = 0; i < layer - 1; i++)
    {
        error[i] = new double[dimension[layer - i - 1]];
    }
}

void Network::FF(double *input)
{
    /* Digitalizing input vector */
    for (int n = 0; n < dimension[0]; n++)
    {
        output[0][n] = static_cast<int>(input[n] * pow(2, numBits));
    }

    /* Feedforward */
    for (int l = 0; l < layer - 1; l++)
    {
        /* n = dimension[l] neurons to m = dimension[l + 1] neurons */

        double **slicedBits = new double *[numBits];
        for (int t = 0; t < numBits; t++)
            slicedBits[t] = new double[dimension[l]];

        /* Bit slicing */
        for (int n = 0; n < dimension[l]; n++)
        {
            for (int t = 0; t < numBits; t++)
            {
                if ((output[l][n] >> t) & 1)
                    slicedBits[t][n] = readVoltage;
                else
                    slicedBits[t][n] = 0;
            }
        }

        /* Read array */
        double totalCurrent[dimension[l + 1]] = {};
        for (int t = 0; t < numBits; t++)
        {
            /* Divide previous total in half */
            for (int m = 0; m < dimension[l + 1]; m++)
                totalCurrent[m] /= 2;

            /* Add new current to total */
            double sumI[dimension[l + 1]];
            double sumRefI = array[l]->ReferenceColumn(slicedBits[t]); // Reference column current
            array[l]->ReadArray(slicedBits[t], sumI);
            for (int m = 0; m < dimension[l + 1]; m++)
                totalCurrent[m] += (sumI[m] - sumRefI); // Subtract reference column current (medium conductance) to make negative weight
        }

        /* Activate */
        for (int m = 0; m < dimension[l + 1]; m++)
            output[l + 1][m] = activation->Activate(totalCurrent[m] * ItoV);
    }
}

void Network::BP(int label)
{
    /* Generate first error vector */
    {
        /* Substract target vector */
        int delta[dimension[layer - 1]] = {}; // Difference between target and network output
        for (int m = 0; m < dimension[layer - 1]; m++)
        {
            delta[m] = output[layer - 1][m];
        }
        delta[label] = output[layer - 1][label] - (pow(2, numBits) - 1);

        /* Hadamard product with differential coefficient vector */
        for (int m = 0; m < dimension[layer - 1]; m++)
        {
            error[0][m] = delta[m] * activation->Derivative(output[layer - 1][m]); // Digital multiplicatoin (FP operation)
        }
    }

    double normFactor = 1 / activation->GetMaxDiff(); // Facotr used to normalize error vector to 1
    double maxWeight = array[0]->GetMaxWeight();      // XXX: Assume all arrays have same maximum weighit (same conductance range)

    /* Backpropagation */
    for (int l = 1; l < layer - 1; l++)
    {
        /* m = dimension[layer - l] neurons to n = dimension[layer - l - 1] neurons */
        double outputVoltageRange = dimension[layer - l] * maxWeight * ItoV * (readVoltage * 2); // Maximum intensity of output voltage

        /* Normalize error vector */
        int normError[dimension[layer - l]]; // Normalized error vector
        for (int m = 0; m < dimension[layer - l]; m++)
            normError[m] = static_cast<int>(error[l - 1][m] * normFactor * pow(2, numBits));

        double **slicedBits = new double *[numBits];
        for (int t = 0; t < numBits; t++)
            slicedBits[t] = new double[dimension[layer - l]];

        /* Bit slicing */
        for (int m = 0; m < dimension[layer - l]; m++)
        {
            for (int t = 0; t < numBits; t++)
            {
                if ((normError[m] >> t) & 1)
                    slicedBits[t][m] = readVoltage;
                else
                    slicedBits[t][m] = 0;
            }
        }

        /* Read array backwaards */
        double totalCurrent[dimension[layer - l - 1]] = {};
        for (int t = 0; t < numBits; t++)
        {
            /* Divide previous total in half */
            for (int n = 0; n < dimension[layer - l - 1]; n++)
                totalCurrent[n] /= 2;

            /* Add new current to total */
            double sumI[dimension[layer - l]];
            double sumRefI = array[layer - l - 1]->ReferenceRow(slicedBits[t]); // Reference row current
            array[layer - l - 1]->ReadArrayBackwards(slicedBits[t], sumI);
            for (int n = 0; n < dimension[layer - l - 1]; n++)
                totalCurrent[n] += (sumI[n] - sumRefI);
        }

        /* Read voltage with linear ramp ADC */
        double outputVoltage[dimension[layer - l - 1]];
        for (int n = 0; n < dimension[layer - l - 1]; n++)
        {
            int step = static_cast<int>(totalCurrent[n] * ItoV / (outputVoltageRange / pow(2, numBits)));
            if (totalCurrent[n] < 0)
                step--; // Because reference ramp voltage increases from negative value
            outputVoltage[n] = step * (outputVoltageRange / pow(2, numBits));
        }

        /* Hadamard product with differential coefficient vector & divide with normalization factor */
        for (int n = 0; n < dimension[layer - l - 1]; n++)
            error[l][n] = outputVoltage[n] * activation->Derivative(output[layer - l - 1][n]) / normFactor;

        /* Next normalization factor */
        normFactor *= 1 / outputVoltageRange / activation->GetMaxDiff();
    }
}

void Network::WeightUpdate(double learningRate, int streamLength)
{
}

Network::~Network()
{
    delete[] dimension;

    for (int i = 0; i < layer; i++)
    {
        delete[] output[i];
    }
    delete[] output;

    for (int i = 0; i < layer - 1; i++)
    {
        delete[] error[i];
    }
    delete[] error;
}