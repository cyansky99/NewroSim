#include "Data.h"
#include <fstream>
#include <iostream>

Data::Data(int numTrain, int numTest, int inputSize)
    : numTrain(numTrain), numTest(numTest), inputSize(inputSize)
{
    trainX = new double *[numTrain];
    for (int i = 0; i < numTrain; i++)
    {
        trainX[i] = new double[inputSize];
    }
    testX = new double *[numTest];
    for (int i = 0; i < numTest; i++)
    {
        testX[i] = new double[inputSize];
    }
    trainY = new int[numTrain];
    testY = new int[numTest];
}

Data::~Data()
{
    for (int i = 0; i < numTrain; i++)
    {
        delete[] trainX[i];
    }
    delete[] trainX;

    for (int i = 0; i < numTest; i++)
    {
        delete[] testX[i];
    }
    delete[] testX;

    delete[] trainY;
    delete[] testY;
}

void Data::ReadData()
{
    /* Read train.txt */
    std::ifstream in("data/train.txt");
    if (in.is_open())
    {
        for (int i = 0; i < numTrain; i++)
        {
            for (int j = 0; j < inputSize; j++)
            {
                in >> trainX[i][j];
            }
        }
    }
    else
    {
        std::cout << "Cannot find train.txt" << std::endl;
        exit(-1);
    }
    /* Read test.txt*/
    in.close();
    in.open("data/test.txt");
    if (in.is_open())
    {
        for (int i = 0; i < numTest; i++)
        {
            for (int j = 0; j < inputSize; j++)
            {
                in >> testX[i][j];
            }
        }
    }
    else
    {
        std::cout << "Cannot find test.txt" << std::endl;
        exit(-1);
    }
    /* Read trainLabel.txt */
    in.close();
    in.open("data/trainLabel.txt");
    if (in.is_open())
    {
        for (int i = 0; i < numTrain; i++)
        {
            in >> trainY[i];
        }
    }
    else
    {
        std::cout << "Cannot find trainLabel.txt" << std::endl;
        exit(-1);
    }
    /* Read testLabel.txt */
    in.close();
    in.open("data/testLabel.txt");
    if (in.is_open())
    {
        for (int i = 0; i < numTest; i++)
        {
            in >> testY[i];
        }
    }
    else
    {
        std::cout << "Cannot find testLabel.txt" << std::endl;
        exit(-1);
    }

    std::cout << "Data Read Complete" << std::endl;
};
