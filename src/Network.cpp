#include "Network.h"
#include <iostream>
#include <cmath>
#include <random>

extern std::mt19937 gen;

Network::Network(int layer, Array **array, Activation *activation, double ItoV, double readVoltage, int ADCsteps)
    : layer(layer), array(array), activation(activation), ItoV(ItoV), readVoltage(readVoltage), ADCsteps(ADCsteps)
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

#pragma omp parallel for
    /* Digitalizing input vector */
    for (int n = 0; n < dimension[0]; n++)
    {
        output[0][n] = static_cast<int>(input[n] * 255 / 256 * pow(2, numBits));
    }

    /* Feedforward */
    for (int l = 0; l < layer - 1; l++)
    {
        /* n = dimension[l] neurons to m = dimension[l + 1] neurons */

        double **slicedBits = new double *[numBits];
#pragma omp parallel for
        for (int t = 0; t < numBits; t++)
            slicedBits[t] = new double[dimension[l]];

#pragma omp parallel for collapse(2)
        /* Bit slicing */
        for (int n = 0; n < dimension[l]; n++)
        {
            for (int t = 0; t < numBits; t++)
            {
                if ((abs(output[l][n]) >> t) & 1)
                    slicedBits[t][n] = (output[l][n] >= 0) ? readVoltage : -readVoltage;
                else
                    slicedBits[t][n] = 0;
            }
        }

        /* Read array */
        double totalCurrent[dimension[l + 1]] = {};
        for (int t = 0; t < numBits; t++)
        {
            double sumI[dimension[l + 1]] = {};
            double sumRefI = array[l]->ReferenceColumn(slicedBits[t]); // Reference column current
            array[l]->ReadArray(slicedBits[t], sumI);
#pragma omp parallel for
            for (int m = 0; m < dimension[l + 1]; m++)
            {
                totalCurrent[m] += (sumI[m] - sumRefI) / pow(2, numBits - t - 1); // Subtract reference column current (medium conductance) to make negative weight}
            }
        }

/* Activate */
#pragma omp parallel for
        for (int m = 0; m < dimension[l + 1]; m++)
            output[l + 1][m] = activation->Activate(totalCurrent[m] * ItoV);

        /* Delete memory Allocation*/
        for (int t = 0; t < numBits; t++)
            delete[] slicedBits[t];
        delete[] slicedBits;
    }
}

void Network::BP(int label)
{
    /* Generate first error vector */
    {
        /* Substract target vector */
        int delta[dimension[layer - 1]] = {}; // Difference between target and network output
#pragma omp parallel for
        for (int m = 0; m < dimension[layer - 1]; m++)
        {
            delta[m] = output[layer - 1][m] - activation->Minimum();
        }
        delta[label] = output[layer - 1][label] - activation->Maximum();

#pragma omp parallel for
        /* Hadamard product with differential coefficient vector */
        for (int m = 0; m < dimension[layer - 1]; m++)
        {
            error[0][m] = delta[m] / pow(2, numBits) * activation->Derivative(output[layer - 1][m]); // Digital multiplicatoin (FP operation)
        }
    }

    double normFactor = 1 / activation->GetMaxDiff(); // Facotr used to normalize error vector to 1
    double maxWeight = array[0]->GetMaxWeight();      // XXX: Assume all arrays have same maximum weighit (same conductance range)

    /* Backpropagation */
    for (int l = 1; l < layer - 1; l++)
    {
        /* m = dimension[layer - l] neurons to n = dimension[layer - l - 1] neurons (array[layer - l - 1]) */
        double outputVoltageRange = dimension[layer - l] * maxWeight * ItoV * (readVoltage * 2); // Maximum intensity of output voltage

        /* Normalize error vector */
        int normError[dimension[layer - l]]; // Normalized error vector
        bool negativeError[dimension[layer - l]] = {};
#pragma omp parallel for
        for (int m = 0; m < dimension[layer - l]; m++)
        {
            if (error[l - 1][m] < 0)
                negativeError[m] = 1;
            normError[m] = abs(static_cast<int>(error[l - 1][m] * normFactor * pow(2, numBits)));
        }

        double **slicedBits = new double *[numBits];
#pragma omp parallel for
        for (int t = 0; t < numBits; t++)
            slicedBits[t] = new double[dimension[layer - l]];

#pragma omp parallel for collapse(2)
        /* Bit slicing */
        for (int m = 0; m < dimension[layer - l]; m++)
        {
            for (int t = 0; t < numBits; t++)
            {
                if ((normError[m] >> t) & 1)
                    slicedBits[t][m] = (negativeError[m]) ? -readVoltage : readVoltage;
                else
                    slicedBits[t][m] = 0;
            }
        }

        /* Read array backwaards */
        double totalCurrent[dimension[layer - l - 1]] = {};
        for (int t = 0; t < numBits; t++)
        {
            double sumI[dimension[layer - l - 1]] = {};
            double sumRefI = array[layer - l - 1]->ReferenceRow(slicedBits[t]); // Reference row current
            array[layer - l - 1]->ReadArrayBackwards(slicedBits[t], sumI);
#pragma omp parallel for
            for (int n = 0; n < dimension[layer - l - 1]; n++)
            {
                totalCurrent[n] += (sumI[n] - sumRefI) / pow(2, numBits - t - 1);
            }
        }

        /* Read voltage with linear ramp ADC */
        double outputVoltage[dimension[layer - l - 1]];
#pragma omp parallel for
        for (int n = 0; n < dimension[layer - l - 1]; n++)
        {
            int step = static_cast<int>(totalCurrent[n] * ItoV / (outputVoltageRange / ADCsteps));
            if (totalCurrent[n] < 0)
                step--; // Because reference ramp voltage increases from negative value
            outputVoltage[n] = step * (outputVoltageRange / ADCsteps);
        }

#pragma omp parallel for
        /* Hadamard product with differential coefficient vector & divide with normalization factor */
        for (int n = 0; n < dimension[layer - l - 1]; n++)
        {
            error[l][n] = outputVoltage[n] * activation->Derivative(output[layer - l - 1][n]) / normFactor;
        }

        /* Next normalization factor */
        normFactor *= 1 / outputVoltageRange / activation->GetMaxDiff();

        /* Delete memory Allocation*/
        for (int t = 0; t < numBits; t++)
            delete[] slicedBits[t];
        delete[] slicedBits;
    }
}

void Network::StochasticPulseWU(double *learningRate, int streamLength, int numLevelLTP, int numLevelLTD)
{
    double maxWeight = array[0]->GetMaxWeight(); // XXX: Assume all arrays have same maximum weighit (same conductance range)

    for (int l = 0; l < layer - 1; l++)
    {
        /* Update weights between layer l and layer l + 1 (array[l]) */
        /* output[l][n] x error[layer - l - 2][m] */

        /* Parallel weight update with coincidence of randomly generated pulses */

        /* Memory allocation and initialization */
        int **numPulse = new int *[dimension[l]]; // Number of coincident pusles
#pragma omp parallel for
        for (int n = 0; n < dimension[l]; n++)
        {
            numPulse[n] = new int[dimension[l + 1]]();
        }

        /* output/error to probability constant */
        double CLTP = learningRate[l] * numLevelLTP / (streamLength * 2 * maxWeight * ItoV);
        double CLTD = learningRate[l] * numLevelLTD / (streamLength * 2 * maxWeight * ItoV);

        /* Generating probabilistic pulse stream of output */
        bool **outputPulseStream = new bool *[dimension[l]];
        bool negativeOutput[dimension[l]] = {};
        if (activation->CanBeNegative())
        {
#pragma omp parallel for
            for (int n = 0; n < dimension[l]; n++)
            {
                if (output[l][n] < 0)
                    negativeOutput[n] = 1;
            }
        }
#pragma omp parallel for
        for (int n = 0; n < dimension[l]; n++)
        {
            outputPulseStream[n] = new bool[streamLength * 2]; // Generate pulse stream twice for positive/negative weight update

            double pLTP = fabs(output[l][n] / pow(2, numBits)); // Pulse probability for LTP
            double pLTD = fabs(output[l][n] / pow(2, numBits)); // Pulse probability for LTD
            // printf("%lf %lf \n", pLTP, pLTD);
            std::bernoulli_distribution disLTP(pLTP);
            std::bernoulli_distribution disLTD(pLTD);

            for (int t = 0; t < streamLength; t++) // First half is for LTP
            {
                outputPulseStream[n][t] = disLTP(gen);
            }

            for (int t = streamLength; t < streamLength * 2; t++) // Second half is for LTD
            {
                outputPulseStream[n][t] = disLTD(gen);
            }
        }

        /* Generating probabilistic pulse stream of error */
        bool **errorPulseStream = new bool *[dimension[l + 1]];
        bool negativeError[dimension[l + 1]] = {};
#pragma omp parallel for
        for (int m = 0; m < dimension[l + 1]; m++)
        {
            if (error[layer - l - 2][m] < 0)
                negativeError[m] = 1;
        }
#pragma omp parallel for
        for (int m = 0; m < dimension[l + 1]; m++)
        {
            errorPulseStream[m] = new bool[streamLength * 2]; // Generate pulse stream twice for positive/negative weight update

            double pLTP = fabs(error[layer - l - 2][m] * CLTP); // Pulse probability for LTP
            double pLTD = fabs(error[layer - l - 2][m] * CLTD); // Pulse probability for LTD
            std::bernoulli_distribution disLTP(pLTP);
            std::bernoulli_distribution disLTD(pLTD);
            for (int t = 0; t < streamLength; t++) // First half is for LTP
            {
                errorPulseStream[m][t] = disLTP(gen);
            }
            for (int t = streamLength; t < streamLength * 2; t++) // Second half is for LTD
            {
                errorPulseStream[m][t] = disLTD(gen);
            }
        }

#pragma omp parallel for collapse(3)
        /* Count coincident pulses */
        for (int t = 0; t < streamLength; t++)
        {
            for (int n = 0; n < dimension[l]; n++)
            {
                for (int m = 0; m < dimension[l + 1]; m++)
                {
                    if (negativeOutput[n] ^ negativeError[m]) // LTP (negative gradient, weight increase)
                    {
                        if (outputPulseStream[n][t] && errorPulseStream[m][t])
                        {
#pragma omp atomic
                            numPulse[n][m]++;
                        }
                    }
                    else // LTD (positive gradient, weight decrease)
                    {
                        if (outputPulseStream[n][t + streamLength] && errorPulseStream[m][t + streamLength])
                        {
#pragma omp atomic
                            numPulse[n][m]--;
                        }
                    }
                }
            }
        }

        /* Weight update with WriteArray */
        array[l]->WriteArray(numPulse);

        /* Delete memory allocaion */
        for (int n = 0; n < dimension[l]; n++)
            delete[] numPulse[n];
        delete[] numPulse;
        for (int n = 0; n < dimension[l]; n++)
            delete[] outputPulseStream[n];
        delete[] outputPulseStream;
        for (int m = 0; m < dimension[l + 1]; m++)
            delete[] errorPulseStream[m];
        delete[] errorPulseStream;
    }
}

void Network::IdealWU(double *learningRate)
{
    for (int l = 0; l < layer - 1; l++)
    {
        double **deltaConductance = new double *[dimension[l]]; // Number of coincident pusles
#pragma omp parallel for
        for (int n = 0; n < dimension[l]; n++)
        {
            deltaConductance[n] = new double[dimension[l + 1]]();
        }

#pragma omp parallel for collapse(2)
        for (int n = 0; n < dimension[l]; n++)
        {
            for (int m = 0; m < dimension[l + 1]; m++)
            {
                deltaConductance[n][m] = -learningRate[l] * error[layer - l - 2][m] * output[l][n] / pow(2, numBits) / ItoV;
            }
        }

        array[l]->IdealWriteArray(deltaConductance);

        for (int n = 0; n < dimension[l]; n++)
            delete[] deltaConductance[n];
        delete[] deltaConductance;
    }
}

void Network::HardwareWU(double *learningRate, double conductanceRange, int numLevelLTP, int numLevelLTD)
{
    for (int l = 0; l < layer - 1; l++)
    {
        int **numPulse = new int *[dimension[l]];
#pragma omp parallel for
        for (int n = 0; n < dimension[l]; n++)
        {
            numPulse[n] = new int[dimension[l + 1]]();
        }

#pragma omp parallel for collapse(2)
        for (int n = 0; n < dimension[l]; n++)
        {
            for (int m = 0; m < dimension[l + 1]; m++)
            {
                double deltaConductance = -learningRate[l] * error[layer - l - 2][m] * output[l][n] / pow(2, numBits) / ItoV;
                numPulse[n][m] = (deltaConductance > 0) ? static_cast<int>(deltaConductance / conductanceRange * numLevelLTP) : static_cast<int>(deltaConductance / conductanceRange * numLevelLTD);
            }
        }

        array[l]->WriteArray(numPulse);

        for (int n = 0; n < dimension[l]; n++)
            delete[] numPulse[n];
        delete[] numPulse;
    }
}

bool Network::Test(int label)
{
    for (int i = 0; i < dimension[layer - 1]; i++)
    {
        if (output[layer - 1][i] >= output[layer - 1][label] && i != label)
            return false;
    }
    return true;
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

void Network::SnapShot(int i)
{
    switch (i)
    {
    case 1:
    {
        for (int l = 0; l < layer; l++)
        {
            printf("[ %d layer output ] :", l + 1);
            for (int n = 0; n < dimension[l]; n++)
            {
                printf(" %d", output[l][n]);
            }
            printf("\n");
        }
        break;
    }
    case 2:
    {
        for (int l = 0; l < layer - 1; l++)
        {
            printf("\n[ %d array weight ]\n\n", l + 1);
            array[l]->PrintArray(ItoV);
        }
        break;
    }
    case 3:
    {
        for (int l = 0; l < layer - 1; l++)
        {
            printf("[ %d layer error ] : ", l + 1);
            for (int n = 0; n < dimension[layer - l - 1]; n++)
            {
                printf("%e ", error[l][n]);
            }
            printf("\n");
        }
        break;
    }
    }
}