#include "TFile.h"
#include "TH1F.h"
#include "RooAddPdf.h"
#include "RooArgSet.h"

#include "ShapeMakerBase.hpp"
#include "ParameterManager.hpp"
#include "ShapeManager.hpp"

// ===========
// Constructor
// ===========
ShapeMakerBase::ShapeMakerBase(std::string name, RooRealVar * x, RooCategory * cat) : 
    m_name(name), 
    m_x(x),
    m_modes(MakeModeList(cat)),
    m_pdf(new RooSimultaneous((name + "_simPdf").c_str(), "", *cat)),
    m_cat(cat),
    m_shapeMade(false) 
{
    m_pars = new ParameterManager(name + "_params");
    m_shapes = new ShapeManager(name + "_shapes", m_x, m_pars);
}


// ==========
// Destructor
// ==========
ShapeMakerBase::~ShapeMakerBase() {
    delete m_pars;
}


// =========
// Get shape
// =========
RooSimultaneous * ShapeMakerBase::Shape() {
    MakeShape();
    return m_pdf;
}


// ============
// Get category
// ============
RooCategory * ShapeMakerBase::Category() {
    return m_cat;
}


// =========================
// Save histograms to a file
// =========================
void ShapeMakerBase::SaveHistograms(std::string filename) {

    // Check shape has been made
    if (!m_shapeMade) {
        std::cout << "Call to SaveHistograms before shape is created." << std::endl;
        std::cout << "Creating shape." << std::endl;
        MakeShape();
    }

    // Open file for saving
    TFile * outfile = TFile::Open(filename.c_str(), "RECREATE");

    // Loop through modes and make histograms
    for (auto mode : m_modes) {

        // Get PDF
        RooAddPdf * pdf = (RooAddPdf*)m_shapes->Get(mode);
        std::cout << "Getting PDF for " << mode << std::endl;
        pdf->Print();
        std::cout << std::endl;

        // Get components and coefficients
        RooArgList * comps = new RooArgList(pdf->pdfList());
        RooArgList * coefs = new RooArgList(pdf->coefList());
        std::cout << "Components: " << std::endl;
        comps->Print();
        std::cout << "Coefficients: " << std::endl;
        coefs->Print();

        // Make each histogram
        double total_yield = 0;
        for (int i = 0; i < comps->getSize(); i++) {

            // Get component and its coefficient
            RooAbsPdf * component = (RooAbsPdf*)comps->at(i);
            RooAbsReal * coef = (RooAbsReal*)coefs->at(i);

            // Get name of component without prefix
            std::string comp_fullname = component->GetName();
            std::string comp_name = comp_fullname.substr(m_shapes->GetName().length() + 1);

            // Make and scale histogram
            TH1F * hist = (TH1F*)component->createHistogram(("hist_" + mode + "_" +
                        comp_name).c_str(),
                    *m_x, RooFit::Binning(10));
            hist->Scale(coef->getVal() * 10 / hist->Integral());
            hist->Write((comp_name + "_" + mode).c_str());

            // Sum total yield
            total_yield += coef->getVal();
        }

        // Make total PDF histogram
        TH1F * hist_total = (TH1F*)pdf->createHistogram(("hist_" + mode).c_str(), 
                *m_x, RooFit::Binning(10));
        hist_total->Scale(total_yield * 10 / hist_total->Integral());
        hist_total->Write(("fit_" + mode).c_str());
    }

    // Save
    outfile->Close();
}


// =====================
// Make simultaneous PDF
// =====================
void ShapeMakerBase::MakeShape() {

    // Set up parameters
    SetConstantParameters();
    SetFloatingParameters();
    SetDependentParameters();

    // Set up shapes
    MakeComponentShapes();
    MakeModeShapes();
    MakeSimultaneousShape();

    // Change flag
    m_shapeMade = true;

}


// ========================================
// Make simultaneous PDF from list of modes
// ========================================
void ShapeMakerBase::MakeSimultaneousShape() {

    // Reset PDF
    delete m_pdf;
    m_pdf = new RooSimultaneous((m_name + "_simPdf").c_str(), "", *m_cat);

    // Loop through modes and add PDFS
    for (auto mode : m_modes) {
        RooAbsPdf * pdf = (RooAbsPdf*)m_shapes->Get(mode);
        m_pdf->addPdf(*pdf, mode.c_str()); 
    }
}


// =====================================
// Make vector of modes from RooCategory
// =====================================
std::vector<std::string> ShapeMakerBase::MakeModeList(RooCategory * cat) {
    std::vector<std::string> list;
    int i = 0;
    while (cat->isValidIndex(i)) {
        cat->setIndex(i);
        list.push_back(cat->getLabel());
        i++;
    }
    return list;
}
