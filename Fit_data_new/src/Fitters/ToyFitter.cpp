#include "TFile.h"
#include "TRandom.h"
#include "TTree.h"

#include "RooDataHist.h"
#include "RooRandom.h"

#include "ToyFitter.hpp"

// ===========
// Constructor
// ===========
ToyFitter::ToyFitter(ShapeMakerBase * toy_maker, std::string filename) :
    m_outfile(TFile::Open(filename.c_str(), "RECREATE")),
    m_tree(new TTree("toy_tree", "")),
    m_toymaker(toy_maker),
    m_toy(GenerateToy(toy_maker)) {}


// ==========
// Destructor
// ==========
ToyFitter::~ToyFitter() {
    m_outfile->cd();
    m_tree->Write();
    m_outfile->Close();
}


// =========================
// Add a PDF to fitting list
// =========================
void ToyFitter::AddFitPdf(ShapeMakerBase * pdf_maker) {
    AddFitPdf(pdf_maker->Name(), pdf_maker);
}


// ==============================================
// Add a fit PDF to fitting list with custom name
// ==============================================
void ToyFitter::AddFitPdf(std::string name, ShapeMakerBase * pdf_maker) {
    
    // Check that pdf maker has same properties as toy maker
    if (pdf_maker->Category() != m_toymaker->Category()) {
        std::cout << "Error: PDF maker (" << pdf_maker->Name() << " and toy maker ("
            << m_toymaker->Name() << " have different RooCategories." << std::endl;
        return;
    } else if (pdf_maker->FitVariable() != m_toymaker->FitVariable()) {
        std::cout << "Error: PDF maker (" << pdf_maker->Name() << " and toy maker ("
            << m_toymaker->Name() << " have different fit variables." << std::endl;
        return;
    }

    // Add to map
    std::cout << "Successfully added PDF maker " << pdf_maker->Name() << 
        " to ToyFitter under key " << name << std::endl;
    m_pdfs.emplace(name, pdf_maker->Shape());
}



// ==============
// Generate a toy
// ==============
RooDataHist * ToyFitter::GenerateToy(ShapeMakerBase * toy_maker) {

    // Set random seed
    TRandom * rand = new TRandom;
    rand->SetSeed();
    RooRandom::setRandomGenerator(rand);

    // Generate a binned dataset
    RooDataHist * data = toy_maker->Shape()->generateBinned(RooArgList(*toy_maker->FitVariable(),
                *toy_maker->Category()), toy_maker->ExpectedEvents());
    return data;

}
