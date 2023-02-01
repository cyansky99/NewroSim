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
    Wire wire(100, 2, 2.3, 2.73e-8);
    Transistor transistor(15e3);

    Array array1(784, 256, &wire, &AgSi, &transistor, 0.0);
    Array array2(256, 128, &wire, &AgSi, &transistor, 0.0);
    Array array3(128, 10, &wire, &AgSi, &transistor, 0.0);

    Array *a[3] = {&array1, &array2, &array3};

    ADCSigmoid activation(4, 4);

    Network network(4, a, &activation, 1e7, 0.5);

    network.FF(data.GetTrainX()[0]);

    std::cout << "Hello, World!\n";
    return 0;
};