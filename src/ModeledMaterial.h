#pragma once

#include "Material.h"

class ModeledMaterial : public Material
{
private:
    const int numLevelLTP;  // Number of conductance states during LTP
    const int numLevelLTD;  // Number of conductance states during LTD
    const double NLLTP;     // Nonlinearity of LTP
    const double NLLTD;     // Nonlinearity of LTD
    const double sigmaDtoD; // Device to device difference

    double LTPA; // Constant A of LTP
    double LTPB; // Constant B of LTP
    double LTDA; // Constant A of LTD
    double LTDB; // Constant B of LTD

public:
    ModeledMaterial(double minConductance, double maxConductance, int numLevelLTP, int numLevelLTD, double NLLTP, double NLLTD, double sigmaDtoD);
    double NewConductance(double conductance, int numPulse) override;
};