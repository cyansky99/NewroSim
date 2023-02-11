#include <iostream>
#include "Data.h"
#include <random>
#include "ModeledMaterial.h"
#include "Cell.h"
#include "Array.h"
#include "Wire.h"
#include "Transistor.h"
#include "Network.h"
#include "ADCSigmoid.h"
#include "IdealSigmoid.h"
#include <cmath>

std::random_device rd;
std::mt19937 gen(rd());

int main()
{

    ModeledMaterial AgSi(3.0769e-9, 3.8462e-8, 97, 100, 0.01, -0.01, 0.0); // 97, 100, 2.4, -4.88
    // Wire wire(100, 2, 2.3, 2.73e-8);
    Wire wire(100, 2, 2.3, 0);
    // Transistor transistor(15e3);
    Transistor transistor(0);

    // Array array(2, 2, &wire, &AgSi, &transistor, 0.0);
    // Array *a = &array;
    // double learningRate = 1.0;
    // ADCSigmoid activation(4, 4);
    // Network network(2, &a, &activation, 1e7, 0.5, 256);
    // double input[2] = {0.1, 0.3};
    // network.FF(input);
    // network.BP(1);
    // network.SnapShot(1);
    // network.SnapShot(2);
    // network.SnapShot(3);
    // network.WeightUpdate(&learningRate, 100, 1000, 1000);
    // network.SnapShot(1);
    // network.SnapShot(2);
    // network.SnapShot(3);

    // Array array1(4, 6, &wire, &AgSi, &transistor, 0.0);
    // Array array2(6, 3, &wire, &AgSi, &transistor, 0.0);
    // Array array3(3, 3, &wire, &AgSi, &transistor, 0.0);
    // Array *a[3] = {&array1, &array2, &array3};

    // double learningRate[3] = {0.1, 0.1, 0.1};
    // ADCSigmoid activation(4, 4);
    // Network network(4, a, &activation, 1e7, 0.5, 1024);
    // double input[4] = {0, 0.25, 0.75, 1};
    // network.FF(input);
    // network.BP(2);
    // network.SnapShot(1);
    // network.SnapShot(2);
    // network.SnapShot(3);
    // network.WeightUpdate(learningRate, 200, 97, 100);

    /* Mnist */
    Data data(60000, 10000, 784);
    data.ReadData();

    Array array1(784, 100, &wire, &AgSi, &transistor, 0.0);
    Array array2(100, 10, &wire, &AgSi, &transistor, 0.0);

    Array *a[2] = {&array1, &array2};

    ADCSigmoid activation(4, 4);
    IdealSigmoid idealActivation(8);

    Network network(3, a, &activation, 1e7, 0.5, 256);

    double learningRate[2] = {0.4, 0.2};
    // 0.63, 0.11, 0.25

    std::uniform_int_distribution<int> dis(0, 59999);

    int num;
    for (int epoch = 0; epoch < 50; epoch++)
    {
        printf("Epoch %d\n", epoch + 1);
        printf("Train Start\n");
        for (int i = 0; i < 10000; i++)
        {
            if (i % 1000 == 999)
                std::cout << "#" << std::flush;
            num = dis(gen);
            network.FF(data.GetTrainX()[num]);
            network.BP(data.GetTrainY()[num]);
            // network.IdealWU(learningRate);
            // network.HardwareWU(learningRate);
            network.WeightUpdate(learningRate, 10, 97, 100);
        }
        printf("\n");
        network.SnapShot(1);
        printf("Test Start\n");
        int cnt = 0;
        for (int j = 0; j < 1000; j++)
        {
            network.FF(data.GetTestX()[j]);
            if (network.Test(data.GetTestY()[j]))
                cnt++;
        }
        std::cout << static_cast<double>(cnt) / 10 << " %" << std::endl;
        if (epoch % 10 == 9)
        {
            for (int i = 0; i < 2; i++)
                learningRate[i] /= 2;
        }
    }
    return 0;
};