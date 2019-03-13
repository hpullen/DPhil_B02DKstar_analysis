#ifndef SYSTEMATIC_FITTER_HPP
#define SYSTEMATIC_FITTER_HPP

#include "DataFitter.hpp"
#include "SysOption.hpp"

class RooRealVar;
class RooCategory;
class TTree;

// ==========================================================================
// Class for fitting with constant parameters shifted, then saving parameters
// ==========================================================================
class SystematicFitter : private DataFitter {

public:
    SystematicFitter(SysOption opt, bool combine_runs, bool split_obs);
          
    ~SystematicFitter();

    void PerformFits(std::string filename, int n_repeats = 1, bool binned = true);

private:
    SysOption m_opt; 
    bool m_combine_runs;
    bool m_split_obs;

    // Make new PDF
    void RemakeShape();

    // Setup for PDF
    RooRealVar * MakeFitVariable();
    RooCategory * MakeCategory(bool combine_runs);

    // Setup for results saving
    std::map<std::string, double*> SetupTree(TTree * tree);
    void PerformSingleFit(std::map<std::string, double*> params_list, bool binned);

    std::vector<std::string> ParsToSave();

};

#endif // SYSTEMATIC_FITTER_HPP
