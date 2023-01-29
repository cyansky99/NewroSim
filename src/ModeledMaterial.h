#pragma once

#include "Material.h"

class ModeledMaterial : public Material
{
private:
    const static int numLevelLTP = 97;     // Number of conductance states during LTP
    const static int numLevelLTD = 100;    // Number of conductance states during LTD
    constexpr static double NLLTP = 2.4;   // Nonlinearity of LTP
    constexpr static double NLLTD = -4.88; // Nonlinearity of LTD
    constexpr static double sigmaDtoD = 0; // Device to device difference

    double LTPA; // Constant A of LTP
    double LTPB; // Constant B of LTP
    double LTDA; // Constant A of LTD
    double LTDB; // Constant B of LTD

public:
    ModeledMaterial();
    double NewConductance(double conductance, int numPulse, int intensity) override;
};