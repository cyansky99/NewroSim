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
#include <cmath>

std::random_device rd;
std::mt19937 gen(rd());

int main()
{
    Data data(60000, 10000, 784);
    data.ReadData();

    ModeledMaterial AgSi(3.0769e-9, 3.8462e-8, 97, 100, 2.4, -4.88, 0.0);
    // Wire wire(100, 2, 2.3, 2.73e-8);
    Wire wire(100, 2, 2.3, 0);
    // Transistor transistor(15e3);
    Transistor transistor(0);

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
    // network.WeightUpdate(learningRate, 200, 97, 100);
    // network.SnapShot(1);

    Array array1(784, 256, &wire, &AgSi, &transistor, 0.0);
    Array array2(256, 128, &wire, &AgSi, &transistor, 0.0);
    Array array3(128, 10, &wire, &AgSi, &transistor, 0.0);

    Array *a[3] = {&array1, &array2, &array3};

    ADCSigmoid activation(4, 4);

    Network network(4, a, &activation, 1e7, 0.5, 1024);

    double learningRate[3] = {0.1, 0.001, 0.0001};
    // 0.63, 0.11, 0.25

    std::uniform_int_distribution<int> dis(0, 59999);

    int num;
    for (int epoch = 0; epoch < 10; epoch++)
    {
        printf("Epoch %d\n", epoch + 1);
        printf("Train Start\n");
        for (int i = 0; i < 10000; i++)
        {
            num = dis(gen);
            network.FF(data.GetTrainX()[num]);
            network.BP(data.GetTrainY()[num]);
            network.WeightUpdate(learningRate, 400, 97, 100);
        }
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
    }
    return 0;
};