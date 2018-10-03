#include "TFile.h"
#include "TTree.h"

#include "RooCategory.h"
#include "RooRealVar.h"
#include "RooFitResult.h"

#include "ToySignificanceFitter.hpp"
#include "ToyPdfMaker.hpp"
#include "DataPdfMaker.hpp"


// ===========
// Constructor
// ===========
ToySignificanceFitter::ToySignificanceFitter() : 
    ToyFitter(MakeToyPdf("piK"), false),
    m_name("twoAndFourBody") {

    // Add fit PDFs
    AddFitPdf(MakeSignalPdf());
    AddFitPdf(MakeNullPdf("piK"));
}
ToySignificanceFitter::ToySignificanceFitter(std::string mode) : 
    ToyFitter(MakeToyPdf(mode), false),
    m_name("twoAndFourBody") {

    AddFitPdf(MakeSignalPdf());
    AddFitPdf(MakeNullPdf(mode));
}



// ==========
// Destructor
// ==========
ToySignificanceFitter::~ToySignificanceFitter() {}


// ==========================================================
// Override standard fitting function to include significance
// ==========================================================
void ToySignificanceFitter::PerformFits(std::string filename, int n_repeats) {

    // Make tree and set up
    TFile * outfile = TFile::Open(filename.c_str(), "RECREATE");
    TTree * tree = new TTree("toy_tree", "");
    std::map<std::string, double*> * params_list = SetupTree(tree);

    // Add a significance branch
    params_list->emplace("significance", new double(0));
    tree->Branch("significance", params_list->at("significance"), "significance/D");

    // Loop over and perform fits
    for (int i = 0; i < n_repeats; i++) {

        // Fit to toy
        std::map<std::string, RooFitResult*> results = PerformSingleFit(params_list);

        // Calculate significance from RooFitResults
        double NLL_signal = results.at(m_name + "_signal")->minNll();
        double NLL_null = results.at(m_name + "_null")->minNll();
        std::cout << "NLL signal: " << NLL_signal << std::endl;
        std::cout << "NLL null: " << NLL_null << std::endl;
        *params_list->at("significance") = sqrt(2 * (NLL_null - NLL_signal));
        std::cout << "Significance: " << *params_list->at("significance")
            << std::endl;

        // Fill tree
        tree->Fill();

        // Generate new toy
        GenerateNewToy();
    }

    // Save plots if only using one repetition
    if (n_repeats == 1) SaveHistograms();

    // Save tree to file
    outfile->cd();
    tree->Write();
    outfile->Close();

}


// =======================
// Make toy generation PDF
// =======================
ShapeMakerBase * ToySignificanceFitter::MakeToyPdf(std::string mode) {
    
    // Initialise mass variable and category
    m_x = new RooRealVar("Bd_ConsD_MD", "", 5000, 5800);
    m_cat = new RooCategory("modes", "");
    for (TString run : {"_run1", "_run2"}) {
        for (TString sign : {"_plus", "_minus"}) {
            m_cat->defineType("Kpi" + run + sign);
            m_cat->defineType("piK" + run + sign);
            m_cat->defineType("KK" + run + sign);
            m_cat->defineType("pipi" + run + sign);
            m_cat->defineType("Kpipipi" + run + sign);
            m_cat->defineType("piKpipi" + run + sign);
            if (run != "_run1") {
                m_cat->defineType("pipipipi" + run + sign);
            }
        }
    }
    std::string input_file = "Results/twoAndFourBody_data_split.root";
    return new ToyPdfMaker("toy", m_x, m_cat, input_file);
}


// ===================
// Make signal fit PDF
// ===================
ShapeMakerBase * ToySignificanceFitter::MakeSignalPdf() {
    DataPdfMaker * pdf = new DataPdfMaker(m_name + "_signal", m_x, m_cat, false);
    return pdf;
}


// =================
// Make null fit PDF
// =================
ShapeMakerBase * ToySignificanceFitter::MakeNullPdf(std::string mode) {
    DataPdfMaker * pdf = new DataPdfMaker(m_name + "_null", m_x, m_cat, false);
    std::string mode_to_zero = (mode == "run1") ? "piK" : mode;
    pdf->SetZeroYield(mode_to_zero);
    return pdf;
}
