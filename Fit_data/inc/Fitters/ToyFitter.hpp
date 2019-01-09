#ifndef TOY_FITTER_HPP
#define TOY_FITTER_HPP

#include <string>
#include <map>

#include "ShapeMakerBase.hpp"

class TFile;
class RooDataHist;
class RooFitResult;

// ===============================================================
// General class for generating and fitting toys using ShapeMakers
// ===============================================================
class ToyFitter {

public: 
    ToyFitter(ShapeMakerBase * toy_maker, bool binned);
    ToyFitter(ShapeMakerBase * toy_maker, bool binned, bool combine_runs);
    virtual ~ToyFitter();

    void AddFitPdf(ShapeMakerBase * pdf_maker);
    void AddFitPdf(std::string name, ShapeMakerBase * pdf_maker);

    virtual void PerformFits(std::string filename, int n_repeats = 1);

protected:
    std::map<std::string, double*> * SetupTree(TTree * tree);
    std::map<std::string, RooFitResult*> PerformSingleFit( 
            std::map<std::string, double*> * params_list);
    void GenerateNewToy();
    void SaveHistograms();
    ShapeMakerBase * m_toymaker;

private:
    std::map<std::string, ShapeMakerBase*> m_pdfs;
    bool m_binned;
    bool m_combine_runs;
    RooAbsData * m_toy;

    RooAbsData * GenerateToy(ShapeMakerBase * toy_maker, bool binned);
};

#endif // TOY_FITTER_HPP
