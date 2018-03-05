#ifndef SYSTEMATIC_FITTER_HPP
#define SYSTEMATIC_FITTER_HPP

#include "DataFitter.hpp"

class RooRealVar;
class RooCategory;
class TTree;

// ==========================================================================
// Class for fitting with constant parameters shifted, then saving parameters
// ==========================================================================
class SystematicFitter : private DataFitter {

public:
    SystematicFitter();
    ~SystematicFitter();

    void PerformFits(std::string filename, int n_repeats = 1);

private:
    // Make new PDF
    void RemakeShape();

    // Setup for PDF
    RooRealVar * MakeFitVariable();
    RooCategory * MakeCategory();

    // Setup for results saving
    std::map<std::string, double*> SetupTree(TTree * tree);
    void PerformSingleFit(std::map<std::string, double*> params_list);

};

#endif // SYSTEMATIC_FITTER_HPP
