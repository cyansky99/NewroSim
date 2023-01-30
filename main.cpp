#include <iostream>
#include "Data.h"
#include <random>
#include "ModeledMaterial.h"
#include "Cell.h"
#include "Array.h"

std::random_device rd;
std::mt19937 gen(rd());

int main()
{
    // Data data(60000, 10000, 784);
    // data.ReadData();

    ModeledMaterial AgSi;
    Material *p = &AgSi;
    Array array(100, 100, p, 1.5, 0.0);

    std::cout << "Hello, World!\n";
    return 0;
};