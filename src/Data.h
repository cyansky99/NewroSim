#pragma once

class Data
{
private:
    int numTrain;    // number of train data
    int numTest;     // number of test data
    int inputSize;   // size of input vector
    double **trainX; // train data feature
    double **testX;  // test data feature
    int *trainY;     // train data label
    int *testY;      // test data lebel

public:
    Data(int numTrain, int numTest, int inputSize);
    ~Data();
    void ReadData();
    double **GetTrainX();
    int *GetTrainY();
    double **GetTestX();
    int *GetTestY();
};