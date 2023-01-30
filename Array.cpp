#include "Array.h"
#include "Cell.h"

Array::Array(int X, int Y, Material *material, double wireResistivity, double readNoise)
    : X(X), Y(Y), wireResistivity(wireResistivity), readNoise(readNoise)
{
    cell = new Cell **[X];

    for (int x = 0; x < X; x++)
    {
        cell[x] = new Cell *[Y];
        for (int y = 0; y < Y; y++)
        {
            cell[x][y] = new Cell(x, y, material);
        }
    }
}

Array::Array(int X, int Y, Material ***material, double wireResistivity, double readNoise)
    : X(X), Y(Y), wireResistivity(wireResistivity), readNoise(readNoise)
{
    cell = new Cell **[X];

    for (int x = 0; x < X; x++)
    {
        cell[x] = new Cell *[Y];
        for (int y = 0; y < Y; y++)
        {
            cell[x][y] = new Cell(x, y, material[x][y]);
        }
    }
}

double *Array::ReadArray(double *voltage)
{
    for (int y = 0; y < Y; y++)
    {
        double sumI = 0;
        for (int x = 0; x < X; x++)
        {
            sumI += cell[x][y]->ReadCell(voltage[x], wireResistivity);
        }
    }
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
}