#ifndef DOUBLE_GAUSSIAN_FITTER_H
#define DOUBLE_GAUSSIAN_FITTER_H

#include "Fitter.hpp"
#include "StartingParamReader.hpp"

// ============================================
// Double Gaussian fitter class for signal peak
// ============================================

class DoubleGaussianFitter : public Fitter {
public:
    DoubleGaussianFitter(std::string name);
    ~DoubleGaussianFitter();

public:
    std::map<std::string, double> fit(TH1F* hist);
    void readStartingParams(std::string paramFile);

private:
    double width1_start, width1_low, width1_high;
    double widthRatio_start, widthRatio_low, widthRatio_high;
    double frac1_start, frac1_low, frac1_high;
    StartingParamReader* m_spr;

    void setupDefaultParams();
};

#endif // DOUBLE_GAUSSIAN_FITTER_H
