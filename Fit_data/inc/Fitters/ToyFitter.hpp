#ifndef TOY_FITTER_HPP
#define TOY_FITTER_HPP

#include <string>
#include <map>

#include "DataPdfMaker.hpp"

class TFile;
class RooDataHist;
class RooFitResult;

// ===============================================================
// General class for generating and fitting toys using ShapeMakers
// ===============================================================
class ToyFitter {

public: 
    ToyFitter(DataPdfMaker * toy_maker, bool binned, bool split_obs = false);
    ToyFitter(DataPdfMaker * toy_maker, bool binned, bool combine_runs, 
            bool split_obs = false);
    virtual ~ToyFitter();

    void AddFitPdf(DataPdfMaker * pdf_maker);
    void AddFitPdf(std::string name, DataPdfMaker * pdf_maker);

    virtual void PerformFits(std::string filename, int n_repeats = 1);

protected:
    std::map<std::string, double*> * SetupTree(TTree * tree);
    std::map<std::string, RooFitResult*> PerformSingleFit( 
            std::map<std::string, double*> * params_list);
    void GenerateNewToy();
    void SaveHistograms();
    DataPdfMaker * m_toymaker;

private:
    std::map<std::string, DataPdfMaker*> m_pdfs;
    RooAbsData * m_toy;
    bool m_binned;
    bool m_combine_runs;
    bool m_split_obs;

    RooAbsData * GenerateToy(DataPdfMaker * toy_maker, bool binned);
};

#endif // TOY_FITTER_HPP
