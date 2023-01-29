#include "ModeledMaterial.h"
#include "FindA.h"
#include <random>
#include <cmath>

extern std::mt19937 gen;

ModeledMaterial::ModeledMaterial()
    : Material(3.0769e-9, 3.8462e-8)
{

    if (sigmaDtoD)
    {
        std::uniform_real_distribution<double> dis(0, sigmaDtoD);
        LTPA = FindA::GetA(NLLTP * (1 + dis(gen)));
        LTDA = FindA::GetA(NLLTD * (1 + dis(gen)));
    }
    else
    {
        LTPA = FindA::GetA(NLLTP);
        LTDA = FindA::GetA(NLLTD);
    }
    LTPB = (maxConductance - minConductance) / (1 - exp(-numLevelLTP / LTPA));
    LTDB = (maxConductance - minConductance) / (1 - exp(-numLevelLTD / LTDA));
}

double ModeledMaterial::NewConductance(double conductance, int numPulse, int intensity)
{
    if (numPulse == 0)
        return conductance;
    else if (numPulse > 0) // LTP
    {
        int xPulse = -LTPA * log(1 - (conductance - minConductance) / LTPB);
        return LTPB * (1 - exp(-(xPulse + numPulse) / LTPA)) + minConductance;
    }
    else // LTD
    {
        int xPulse = -LTDA * log(1 - (conductance - minConductance) / LTDB);
        return LTDB * (1 - exp(-(xPulse + numPulse) / LTDA)) + minConductance;
    }
}