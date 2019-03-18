#ifndef TOY_SYSTEMATIC_FITTER_HPP
#define TOY_SYSTEMATIC_FITTER_HPP

#include "ToyFitter.hpp"
#include "SysOption.hpp"
class ToyPdfMaker;

// ===========================================================================
// Class for generating a toy and fitting it with a normal and null hypothesis
// ===========================================================================
class ToySystematicFitter : private ToyFitter {

public: 
    ToySystematicFitter(SysOption opt, bool combine_runs, bool split_obs);
    ~ToySystematicFitter();
    
    void PerformFits(std::string filename, int n_repeats = 1);

private:
    RooCategory * m_cat;
    RooRealVar * m_x;
    std::string m_name;
    SysOption m_opt;
    bool m_combine_runs;
    bool m_split_obs;

    ToyPdfMaker * MakeToyPdf(std::map<std::string, double*> extra_events,
            bool combine_runs, bool split_obs);
    ShapeMakerBase * MakeSignalPdf();

};

#endif // TOY_SYSTEMATIC_FITTER_HPP
