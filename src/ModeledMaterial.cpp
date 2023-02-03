#include "ModeledMaterial.h"
#include "FindA.h"
#include <random>
#include <cmath>

extern std::mt19937 gen;

ModeledMaterial::ModeledMaterial(double minConductance, double maxConductance, int numLevelLTP, int numLevelLTD, double NLLTP, double NLLTD, double sigmaDtoD)
    : Material(minConductance, maxConductance), numLevelLTP(numLevelLTP), numLevelLTD(numLevelLTD), NLLTP(NLLTP), NLLTD(NLLTD), sigmaDtoD(sigmaDtoD)
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

double ModeledMaterial::NewConductance(double conductance, int numPulse)
{
    if (numPulse == 0)
        return conductance;
    else if (numPulse > 0) // LTP
    {
        int xPulse = -LTPA * log(1 - (conductance - minConductance) / LTPB);
        // printf("LTP %d %d\n", xPulse, numPulse); // TODO: delete afeter debuggging
        double newConductance = LTPB * (1 - exp(-(xPulse + numPulse) / LTPA)) + minConductance;
        if (newConductance > maxConductance)
            return maxConductance;
        else
            return newConductance;
    }
    else // LTD
    {
        int xPulse = -LTDA * log(1 - (conductance - minConductance) / LTDB);
        // printf("LTD %d %d\n", xPulse, numPulse); // TODO: delete afeter debuggging
        double newConductance = LTDB * (1 - exp(-(xPulse + numPulse) / LTDA)) + minConductance;
        if (newConductance < minConductance)
            return minConductance;
        else
            return newConductance;
    }
}