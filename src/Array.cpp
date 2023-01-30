#include "Array.h"
#include "Cell.h"

Array::Array(int X, int Y, Material *material) : X(X), Y(Y)
{
    cell = new Cell **[X];

    for (int x = 0; x < X; x++)
    {
        cell[x] = new Cell *[Y];
        for (int y = 0; y < Y; y++)
        {
            cell[x][y] = &Cell(x, y, material);
        }
    }
}

Array::Array(int X, int Y, Material ***material) : X(X), Y(Y)
{
}

Array::~Array()
{
    for (int x = 0; x < X; x++)
        delete[] cell[x];
    delete[] cell;
}