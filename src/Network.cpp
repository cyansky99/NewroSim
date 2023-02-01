#include "Network.h"
#include <iostream>
#include <cmath>

Network::Network(int layer, Array **array, Activation *activation) : layer(layer), array(array), activation(activation)
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

    /* Mermory allocation for members */
    output = new int *[layer];
    for (int i = 0; i < layer; i++)
    {
        output[i] = new int[dimension[i]];
    }
    error = new double *[layer - 1];
    for (int i = 0; i < layer - 1; i++)
    {
        error[i] = new double[dimension[i + 1]];
    }
}

void Network::FF(double *input, double readVoltage, double ItoV)
{
    int numBits = activation->GetOuputBits();

    /* Digitalizing input vector */
    for (int n = 0; n < dimension[0]; n++)
    {
        output[0][n] = static_cast<int>(input[n] * pow(2, numBits));
    }

    /* Feedforward */
    for (int l = 0; l < layer - 1; l++)
    {
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

        /* Read array*/
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
                totalCurrent[m] += (sumI[m] - sumRefI);
        }

        /* Activate */
        for (int m = 0; m < dimension[l + 1]; m++)
        {
            output[l + 1][m] = activation->Activate(totalCurrent[m] * ItoV);
            printf("%d ", output[l + 1][m]);
        }
        printf("\n");
    }
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