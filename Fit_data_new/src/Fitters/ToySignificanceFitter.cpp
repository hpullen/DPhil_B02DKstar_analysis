#include "TFile.h"
#include "TTree.h"

#include "RooCategory.h"
#include "RooRealVar.h"
#include "RooFitResult.h"

#include "ToySignificanceFitter.hpp"
#include "SimpleToyMaker.hpp"
#include "SimplePdfMaker.hpp"

// ===========
// Constructor
// ===========
ToySignificanceFitter::ToySignificanceFitter() : ToyFitter(MakeToyPdf()) {

    // Add fit PDFs
    AddFitPdf("signal", MakeSignalPdf());
    AddFitPdf("null", MakeNullPdf());
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
    std::map<std::string, double*> params_list = SetupTree(tree);

    // Add a significance branch
    params_list.emplace("significance", new double(0));
    tree->Branch("significance", params_list["significance"], "significance/D");

    // Loop over and perform fits
    for (int i = 0; i < n_repeats; i++) {

        // Fit to toy
        std::map<std::string, RooFitResult*> results = PerformSingleFit(params_list);

        // Calculate significance from RooFitResults
        double NLL_signal = results.at("signal")->minNll();
        double NLL_null = results.at("null")->minNll();
        std::cout << "NLL signal: " << NLL_signal << std::endl;
        std::cout << "NLL null: " << NLL_null << std::endl;
        *params_list.at("significance") = sqrt(2 * (NLL_null - NLL_signal));
        std::cout << "Significance: " << *params_list.at("significance")
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
ShapeMakerBase * ToySignificanceFitter::MakeToyPdf() {
    
    // Initialise mass variable and category
    m_x = new RooRealVar("Bd_M", "", 5000, 5800);
    m_cat = new RooCategory("modes", "");
    m_cat->defineType("Kpi");
    m_cat->defineType("piK");

    // Make toy PDF
    return new SimpleToyMaker("toy", m_x, m_cat);
}


// ===================
// Make signal fit PDF
// ===================
ShapeMakerBase * ToySignificanceFitter::MakeSignalPdf() {
    return new SimplePdfMaker("signal", m_x, m_cat);
}


// =================
// Make null fit PDF
// =================
ShapeMakerBase * ToySignificanceFitter::MakeNullPdf() {
    return new SimplePdfMaker("null", m_x, m_cat, Hypothesis::Null);
}
