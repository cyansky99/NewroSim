#include "Material.h"
#include <random>

extern std::mt19937 gen;

Material::Material(double minConductance, double maxConductance)
    : minConductance(minConductance), maxConductance(maxConductance)
{
}

double Material::RandomConductance()
{
    std::uniform_real_distribution<double> dis(0.0, 1.0);
    return minConductance + (maxConductance - minConductance) * dis(gen);
}

double Material::MinConductance()
{
    return minConductance;
}

double Material::MaxConductance()
{
    return maxConductance;
}