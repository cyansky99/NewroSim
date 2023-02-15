#include <iostream>
#include <cmath>
#include <algorithm>
#include <random>

#include "Data.h"
#include "ModeledMaterial.h"
#include "Cell.h"
#include "Array.h"
#include "Wire.h"
#include "Transistor.h"
#include "Network.h"
#include "ADCSigmoid.h"
#include "IdealSigmoid.h"

std::random_device rd;
std::mt19937 gen(rd());

/* Parameter */

/* Material */
#define MINCONDUCTANCE 3.0769e-9
#define MAXCONDUCTANCE 3.8462e-8
#define NUMLEVELLTP 97
#define NUMLEVELLTD 100
#define NLLTP 2.4
#define NLLTD -4.88
/* Wire */
#define WIREWIDTH 100
#define CELLSIZE 2
#define WIREASPECTRATIO 2.3
#define WIRERESISTIVITY 2.73e-8
/* Transistor */
#define ONRESISTANCE 15e3
/* Data */
#define NUMTRAINDATA 60000
#define NUMTESTDATA 10000
#define INPUTSIZE 784
#define OUTPUTSIZE 10
/* Network */
#define LAYER 4
#define READVOLTAGE 0.5
#define BPADCNUMLEVEL 4096
#define MAXWEIGHT 1.0
/* Train */
#define EPOCH 100
#define LRDEC 2.0
#define LRDECCYCLE 10
/* Stochastic pulse weight update */
#define STREAMLENGTH 10

int main()
{

    ModeledMaterial AgSi(MINCONDUCTANCE, MAXCONDUCTANCE, NUMLEVELLTP, NUMLEVELLTD, NLLTP, NLLTD, 0.0); // TODO: SigmaDtoD
    Wire wire(WIREWIDTH, CELLSIZE, WIREASPECTRATIO, WIRERESISTIVITY);
    Transistor transistor(ONRESISTANCE);

    /* Mnist */
    Data data(NUMTRAINDATA, NUMTESTDATA, INPUTSIZE);
    data.ReadData();

    Array inputArray(INPUTSIZE, 256, &wire, &AgSi, &transistor, 0.0);
    Array hiddenArray(256, 128, &wire, &AgSi, &transistor, 0.0);
    Array outputArray(128, OUTPUTSIZE, &wire, &AgSi, &transistor, 0.0);

    Array *a[LAYER - 1] = {&inputArray, &hiddenArray, &outputArray};

    ADCSigmoid activation(6, 2);

    double ItoV = MAXWEIGHT / (MAXCONDUCTANCE - MINCONDUCTANCE) / (2 * READVOLTAGE);
    Network network(LAYER, a, &activation, ItoV, READVOLTAGE, BPADCNUMLEVEL);

    double learningRate[LAYER - 1] = {0.4, 0.001, 0.00007};
    // 0.4, 0.001, 0.00007

    // std::uniform_int_distribution<int> dis(0, NUMTRAINDATA -1);
    // int num;

    std::vector<int> index(NUMTRAINDATA);
    std::iota(index.begin(), index.end(), 0);

    for (int epoch = 0; epoch < EPOCH; epoch++)
    {
        printf("\n[ Epoch %d ]\n", epoch + 1);
        printf("> Train Start\n");

        for (int i = 0; i < NUMTRAINDATA / 1000; i++)
            printf("-");
        printf("\n");

        std::shuffle(index.begin(), index.end(), gen);
        for (int i = 0; i < NUMTRAINDATA; i++)
        {
            if (i % 1000 == 999)
                std::cout << "#" << std::flush;
            // num = dis(gen);
            network.FF(data.GetTrainX()[index[i]]);
            network.BP(data.GetTrainY()[index[i]]);
            // network.IdealWU(learningRate);
            // network.HardwareWU(learningRate, MAXCONDUCTANCE - MINCONDUCTANCE, NUMLEVELLTP, NUMLEVELLTD);
            network.StochasticPulseWU(learningRate, STREAMLENGTH, NUMLEVELLTP, NUMLEVELLTD);
        }
        printf("\n");

        network.SnapShot(1);
        printf("> Test Start\n");
        int cnt = 0;
        for (int i = 0; i < NUMTESTDATA / 1000; i++)
            printf("-");
        printf("\n");
        for (int i = 0; i < NUMTESTDATA; i++)
        {
            if (i % 1000 == 999)
                std::cout << "#" << std::flush;
            network.FF(data.GetTestX()[i]);
            if (network.Test(data.GetTestY()[i]))
                cnt++;
        }
        printf("\n");
        std::cout << "> Accuracy " << static_cast<double>(cnt) / NUMTESTDATA * 100 << " %" << std::endl;
        if (epoch % LRDECCYCLE == LRDECCYCLE - 1)
        {
            for (int i = 0; i < LAYER - 1; i++)
                learningRate[i] /= LRDEC;
        }
    }
    return 0;
};