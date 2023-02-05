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
        refCol[x] = new Cell(x, Y, transistor, (material->MinConductance() + material->MaxConductance()) / 2);

    /* Make reference row */
    refRow = new Cell *[Y];
    for (int y = 0; y < Y; y++)
        refRow[y] = new Cell(X, y, transistor, (material->MinConductance() + material->MaxConductance()) / 2);

    /* Get maximum weight */
    maxWeight = (material->MaxConductance() - material->MinConductance()) / 2;
}

Array::Array(int X, int Y, Wire *wire, Material ***material, Transistor *transistor, double readNoise)
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
#pragma omp parallel for
    for (int y = 0; y < Y; y++)
    {
        double sumI = 0;
        for (int x = 0; x < X; x++)
        {
            sumI += cell[x][y]->ReadCell(voltage[x], wire->UnitResistance(), readNoise);
        }
        current[y] = sumI;
    }
}

void Array::ReadArrayBackwards(double *voltage, double *current)
{
    // #pragma omp parallel for
    for (int x = 0; x < X; x++)
    {
        double sumI = 0;
        for (int y = 0; y < Y; y++)
        {
            sumI += cell[x][y]->ReadCell(voltage[y], wire->UnitResistance(), readNoise);
            printf("%d,%d: %e \n", x, y, cell[x][y]->conductance);
        }

        printf("\n");
        PrintArray(1e7);
        current[x] = sumI;
    }
}

double Array::ReferenceColumn(double *voltage)
{
    double sumI = 0;
    for (int x = 0; x < X; x++)
    {
        sumI += refCol[x]->ReadCell(voltage[x], wire->UnitResistance(), readNoise);
    }
    return sumI;
}

double Array::ReferenceRow(double *voltage)
{
    double sumI = 0;
    for (int y = 0; y < Y; y++)
    {
        sumI += refRow[y]->ReadCell(voltage[y], wire->UnitResistance(), readNoise);
    }
    return sumI;
}

void Array::WriteArray(int **numPulse)
{
    for (int x = 0; x < X; x++)
    {
        for (int y = 0; y < Y; y++)
            cell[x][y]->WriteCell(numPulse[x][y]);
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
    for (int y = 0; y < Y; y++)
        delete refRow[y];
}

void Array::PrintArray(double scale) // TODO: delete after debugging
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