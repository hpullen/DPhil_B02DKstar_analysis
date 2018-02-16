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
    ToyFitter(ShapeMakerBase * toy_maker, std::string filename);
    ~ToyFitter();

    void AddFitPdf(ShapeMakerBase * pdf_maker);
    void AddFitPdf(std::string name, ShapeMakerBase * pdf_maker);

    void PerformFits(int n_repeats = 1);

private:
    RooDataHist * GenerateToy(ShapeMakerBase * toy_maker);

    TFile * m_outfile;
    TTree * m_tree;
    ShapeMakerBase * m_toymaker;
    RooDataHist * m_toy;
    std::map<std::string, RooSimultaneous*> m_pdfs;
};

#endif // TOY_FITTER_HPP
