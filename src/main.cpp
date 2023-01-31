#include <iostream>
#include "Data.h"
#include <random>
#include "ModeledMaterial.h"
#include "Cell.h"
#include "Array.h"
#include "Wire.h"
#include "Transistor.h"

std::random_device rd;
std::mt19937 gen(rd());

int main()
{
    // Data data(60000, 10000, 784);
    // data.ReadData();

    ModeledMaterial AgSi(3.0769e-9, 3.8462e-8, 97, 100, 2.4, -4.88, 0.0);
    Wire wire(100, 2, 2.3, 2.73e-8);
    Transistor transistor(15e3);

    Array array(100, 100, &wire, &AgSi, &transistor, 0.0);

    std::cout << "Hello, World!\n";
    return 0;
};