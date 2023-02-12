#include "Array.h"
#include "Cell.h"
#include <iostream>

Array::Array(int X, int Y, Wire *wire, Material *material, Transistor *transistor, double readNoise)
    : X(X), Y(Y), wire(wire), readNoise(readNoise)
{
    cell = new Cell **[X];
    for (int x = 0; x < X; x++)
    {
        cell[x] = new Cell *[Y];
        for (int y = 0; y < Y; y++)
        {
            cell[x][y] = new Cell(x, y, material, transistor);
        }
    }

    /* Make reference column */
    refCol = new Cell *[X];
    for (int x = 0; x < X; x++)
        refCol[x] = new Cell(x, Y, material, transistor, (material->MinConductance() + material->MaxConductance()) / 2);

    /* Make reference row */
    refRow = new Cell *[Y];
    for (int y = 0; y < Y; y++)
        refRow[y] = new Cell(X, y, material, transistor, (material->MinConductance() + material->MaxConductance()) / 2);

    /* Get maximum weight */
    maxWeight = (material->MaxConductance() - material->MinConductance()) / 2;

    wireResistance = wire->UnitResistance();
}

Array::Array(int X, int Y, Wire *wire, Material ***material, Transistor *transistor, double readNoise) // TODO: add reference column/row and wire resistance, max weight
    : X(X), Y(Y), wire(wire), readNoise(readNoise)
{
    cell = new Cell **[X];

    for (int x = 0; x < X; x++)
    {
        cell[x] = new Cell *[Y];
        for (int y = 0; y < Y; y++)
        {
            cell[x][y] = new Cell(x, y, material[x][y], transistor);
        }
    }
}

int Array::GetX()
{
    return X;
}

int Array::GetY()
{
    return Y;
}

void Array::ReadArray(double *voltage, double *current)
{
#pragma omp parallel for collapse(2) reduction(+ \
                                               : current[:Y])
    for (int y = 0; y < Y; y++)
    {
        for (int x = 0; x < X; x++)
        {
            current[y] += cell[x][y]->ReadCell(voltage[x], wireResistance, readNoise);
        }
    }
}

void Array::ReadArrayBackwards(double *voltage, double *current)
{
#pragma omp parallel for collapse(2) reduction(+ \
                                               : current[:X])
    for (int x = 0; x < X; x++)
    {
        for (int y = 0; y < Y; y++)
        {
            current[x] += cell[x][y]->ReadCell(voltage[y], wireResistance, readNoise);
        }
    }
}

double Array::ReferenceColumn(double *voltage)
{
    double sumI = 0;
#pragma omp parallel for reduction(+ \
                                   : sumI)
    for (int x = 0; x < X; x++)
    {
        sumI += refCol[x]->ReadCell(voltage[x], wireResistance, readNoise);
    }
    return sumI;
}

double Array::ReferenceRow(double *voltage)
{
    double sumI = 0;
#pragma omp parallel for reduction(+ \
                                   : sumI)
    for (int y = 0; y < Y; y++)
    {
        sumI += refRow[y]->ReadCell(voltage[y], wireResistance, readNoise);
    }
    return sumI;
}

void Array::WriteArray(int **numPulse)
{
#pragma omp parallel for collapse(2)
    for (int x = 0; x < X; x++)
    {
        for (int y = 0; y < Y; y++)
            cell[x][y]->WriteCell(numPulse[x][y]);
    }
}

void Array::IdealWriteArray(double **deltaConductance)
{
#pragma omp parallel for collapse(2)
    for (int x = 0; x < X; x++)
    {
        for (int y = 0; y < Y; y++)
            cell[x][y]->IdealWriteCell(deltaConductance[x][y]);
    }
}

double Array::GetMaxWeight()
{
    return maxWeight;
}

Array::~Array()
{
    for (int x = 0; x < X; x++)
    {
        for (int y = 0; y < Y; y++)
            delete cell[x][y];
        delete[] cell[x];
    }
    delete[] cell;

    for (int x = 0; x < X; x++)
        delete refCol[x];
    delete[] refCol;
    for (int y = 0; y < Y; y++)
        delete refRow[y];
    delete[] refRow;
}

void Array::PrintArray(double scale)
{
    for (int x = X - 1; x >= 0; x--)
    {
        for (int y = 0; y < Y; y++)
        {
            cell[x][y]->PrintCell(scale);
            printf(" ");
        }
        printf("\n");
    }
}