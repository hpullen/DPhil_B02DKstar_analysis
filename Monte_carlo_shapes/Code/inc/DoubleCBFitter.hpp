#ifndef DOUBLE_CB_FITTER_H
#define DOUBLE_CB_FITTER_H

#include "Fitter.hpp"

// ============================================
// Double Crystal Ball fitter class for signal peak
// ============================================

class DoubleCBFitter : public Fitter {
public:
    DoubleCBFitter(std::string name);
    ~DoubleCBFitter();

public:
    std::map<std::string, double> fit(TH1F* hist);
    void readStartingParams(std::string paramFile);

private:
    double mean_start, mean_low, mean_high;
    double sigma1_start, sigma1_low, sigma1_high;
    double alpha1_start, alpha1_low, alpha1_high;
    double alpha2_start, alpha2_low, alpha2_high;
    double frac1_start, frac1_low, frac1_high;

    void setupDefaultParams();

};

#endif // DOUBLE_CB_FITTER_H
