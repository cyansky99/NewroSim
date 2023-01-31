#include "Material.h"
#include <random>

extern std::mt19937 gen;

double Material::RandomConductance()
{
    std::uniform_real_distribution<double> dis(0.0, 1.0);
    return minConductance + (maxConductance - minConductance) * dis(gen);
}