#include "Array.h"
#include "Cell.h"

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

void Array::ReadArray(double *voltage, double *current)
{
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
    for (int x = 0; x < X; x++)
    {
        double sumI = 0;
        for (int y = 0; y < Y; y++)
        {
            sumI += cell[x][y]->ReadCell(voltage[y], wire->UnitResistance(), readNoise);
        }
        current[x] = sumI;
    }
}

void Array::WriteArray(int **numPulse)
{
    for (int x = 0; x < X; x++)
    {
        for (int y = 0; y < Y; y++)
            cell[x][y]->WriteCell(numPulse[x][y]);
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