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
    ToyFitter(MakeToyPdf()),
    m_modeToZero("piK"),
    m_name("twoAndFourBody") {

    // Add fit PDFs
    AddFitPdf(MakeSignalPdf());
    AddFitPdf(MakeNullPdf());
}
ToySignificanceFitter::ToySignificanceFitter(std::string mode) : 
    ToyFitter(MakeToyPdf()),
    m_modeToZero(mode),
    m_name("twoAndFourBody") {

    // Add fit PDFs
    AddFitPdf(MakeSignalPdf());
    AddFitPdf(MakeNullPdf());
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
        double NLL_signal = results.at(m_name + "_signal")->minNll();
        double NLL_null = results.at(m_name + "_null")->minNll();
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
    m_x = new RooRealVar("Bd_ConsD_MD", "", 5000, 5800);
    m_cat = new RooCategory("modes", "");
    m_cat->defineType("Kpi");
    m_cat->defineType("piK");
    m_cat->defineType("KK");
    m_cat->defineType("pipi");
    m_cat->defineType("Kpipipi");
    m_cat->defineType("piKpipi");
    m_cat->defineType("pipipipi");
    return new ToyPdfMaker("toy", m_x, m_cat, 
            "Results/twoAndFourBody_data.root");
}


// ===================
// Make signal fit PDF
// ===================
ShapeMakerBase * ToySignificanceFitter::MakeSignalPdf() {
    return new DataPdfMaker(m_name + "_signal", m_x, m_cat, false);
}


// =================
// Make null fit PDF
// =================
ShapeMakerBase * ToySignificanceFitter::MakeNullPdf() {
    DataPdfMaker * pdf = new DataPdfMaker(m_name + "_null", m_x, m_cat, false);
    pdf->SetZeroYield(m_modeToZero);
    return pdf;
}
