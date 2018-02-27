#include "TFile.h"
#include "TStyle.h"
#include "TH1F.h"

#include "RooAddPdf.h"
#include "RooArgSet.h"
#include "RooDataHist.h"
#include "RooHist.h"
#include "RooPlot.h"

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
    m_x->Print();
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
    if (!m_shapeMade) MakeShape();
    return m_pdf;
}


// ============
// Get category
// ============
RooCategory * ShapeMakerBase::Category() {
    return m_cat;
}


// ====================
// Get fitting variable
// ====================
RooRealVar * ShapeMakerBase::FitVariable() {
    return m_x;
}


// =============================
// Get expected number of events
// =============================
int ShapeMakerBase::ExpectedEvents() {
    return Shape()->expectedEvents(*m_cat);
}


// ========
// Get name
// ========
std::string ShapeMakerBase::Name() {
    return m_name;
}


// =================================
// Get vector listing the parameters
// =================================
std::vector<std::string> ShapeMakerBase::Parameters() {

    // Return if already made
    if (m_parList.size() > 0) return m_parList;

    // Make sure shape is made
    if (!m_shapeMade) {
        MakeShape();
    }

    // Get free parameters of shape
    std::vector<std::string> par_list;
    RooArgSet * pars = m_pdf->getParameters(RooArgList(*m_x, *m_cat));
    TIterator * it = pars->createIterator();
    RooRealVar * par;
    while ((par = (RooRealVar*)it->Next())) {
        if (!par->isConstant()) {
            std::string fullname = par->GetName();
            std::string name = fullname.substr((m_name + "_params_").length());
            par_list.push_back(name);
        }
    }
    m_parList = par_list;
    return par_list;
}


// =========================================
// Remake shape with its original parameters
// =========================================
RooSimultaneous * ShapeMakerBase::RemakeShape() {

    // If shape hasn't been made for the first time, make it
    if (!m_shapeMade) {
        MakeShape();
        return m_pdf;
    }

    std::cout << "REMAKING SHAPE" << std::endl;
    
    // Turn off overwrite warnings in managers
    m_pars->SetWarnings(false);
    m_shapes->SetWarnings(false);

    // Remake the shape
    MakeShape();

    // Turn warnings back on
    m_pars->SetWarnings(true);
    m_shapes->SetWarnings(true);

    // Return the remade shape
    return m_pdf;
}


// =========================
// Save histograms to a file
// =========================
void ShapeMakerBase::SaveHistograms(std::string filename, bool blind) {

    // Check shape has been made
    if (!m_shapeMade) {
        std::cout << "Call to SaveHistograms before shape is created." << std::endl;
        std::cout << "Creating shape." << std::endl;
        MakeShape();
    }

    // Open file for saving
    TFile * outfile = TFile::Open(filename.c_str(), "RECREATE");

    // Save fit shapes
    SaveFitShapes(outfile, blind);

    // Save
    outfile->Close();
}


// ========================================
// Save histograms to a file including data
// ========================================
void ShapeMakerBase::SaveHistograms(std::string filename, RooDataHist * data,
        bool blind) {

    // Check shape has been made
    if (!m_shapeMade) {
        std::cout << "Call to SaveHistograms before shape is created." << std::endl;
        std::cout << "Creating shape." << std::endl;
        MakeShape();
    }

    // Open file for saving
    TFile * outfile = TFile::Open(filename.c_str(), "RECREATE");

    // Save fit shapes
    SaveFitShapes(outfile, blind);

    // Save data and pulls for each mode
    std::string cat_name = m_cat->GetName();
    for (auto mode : m_modes) {

        // Make data histogram
        RooDataHist * mode_data = (RooDataHist*)data->reduce((cat_name + "==" + 
               cat_name + "::" + mode).c_str());
        TH1F * mode_hist = (TH1F*)mode_data->createHistogram(("data_hist_" + 
                    mode).c_str(), *m_x, RooFit::Binning(10 * m_x->getBins()));

        // Blind if needed
        if (blind && mode != "Kpi") {
            const double B_mass = 5279.61;
            const double blind_region = 50;
            for (int bin = 1; bin < mode_hist->GetNbinsX(); bin++) {
                if (abs(mode_hist->GetBinCenter(bin) - B_mass) < blind_region) {
                    mode_hist->SetBinContent(bin, 0);
                    mode_hist->SetBinError(bin, 0);
                }
            }
        }

        // Write to file
        outfile->cd();
        mode_hist->Write(("data_" + mode).c_str());

        // Save pulls 
        RooPlot * frame = m_x->frame();
        mode_data->plotOn(frame);
        m_shapes->Get(mode)->plotOn(frame);
        RooHist * pulls = frame->pullHist();
        pulls->Write(("pulls_" + mode).c_str());
    }


    // Save
    outfile->Close();
}

// =====================
// Make simultaneous PDF
// =====================
void ShapeMakerBase::MakeShape() {

    // Reset managers
    m_pars->Reset();
    m_shapes->Reset();

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


// ===============
// Save fit shapes
// ===============
void ShapeMakerBase::SaveFitShapes(TFile * file, bool blind) {

    // Loop through modes and make histograms
    for (auto mode : m_modes) {
        SaveSingleFitShape(mode, file, (blind && mode != "Kpi"));
    }
}


// ==================================
// Save a single fit shape for a mode
// ==================================
void ShapeMakerBase::SaveSingleFitShape(std::string mode, TFile * file, bool blind) 
{

    // Parameters for blinding
    const double B_mass = 5279.61;
    const double blind_region = 50;

    // Get PDF
    RooAddPdf * pdf = (RooAddPdf*)m_shapes->Get(mode);

    // Get components and coefficients
    RooArgList * comps = new RooArgList(pdf->pdfList());
    RooArgList * coefs = new RooArgList(pdf->coefList());

    // Make each histogram
    double total_yield = 0;
    for (int i = 0; i < comps->getSize(); i++) {

        // Get component and its coefficient
        RooAbsPdf * component = (RooAbsPdf*)comps->at(i);
        RooAbsReal * coef = (RooAbsReal*)coefs->at(i);

        // Get name of component without prefix
        std::string comp_fullname = component->GetName();
        std::string comp_name = comp_fullname.substr(m_shapes->GetName().length() 
                + 1);

        // Make and scale histogram
        TH1F * hist = (TH1F*)component->createHistogram(("hist_" + mode + "_" +
                    comp_name).c_str(), *m_x, RooFit::Binning(10 * m_x->getBins()));
        hist->Scale(coef->getVal() * 10 / hist->Integral());

        // Remove central bins if blinding
        if (blind) {
            for (int bin = 1; bin < hist->GetNbinsX(); bin++) {
                if (abs(hist->GetBinCenter(bin) - B_mass) < blind_region) {
                    hist->SetBinContent(bin, 0);
                }
            }
        }

        // Save to file
        file->cd();
        if (comp_name.find("_" + mode) == std::string::npos) {

            // Check component name doesn't contain another mode
            bool written = false;
            for (auto another_mode : m_modes) {
                std::size_t pos = comp_name.find("_" + another_mode);
                if (pos != std::string::npos) {
                    hist->Write((comp_name.substr(0, pos) + "_" + mode).c_str());
                    written = true;
                }
            } 
            // Add mode to name and write
            if (!written) hist->Write((comp_name + "_" + mode).c_str());

        } else {
            hist->Write(comp_name.c_str());
        }

        // Sum total yield
        total_yield += coef->getVal();
    }

    // Make total PDF histogram
    TH1F * hist_total = (TH1F*)pdf->createHistogram(("hist_" + mode).c_str(), 
            *m_x, RooFit::Binning(10 * m_x->getBins()));
    hist_total->Scale(total_yield * 10 / hist_total->Integral());
    if (blind) {
        for (int bin = 1; bin < hist_total->GetNbinsX(); bin++) {
            if (abs(hist_total->GetBinCenter(bin) - B_mass) < blind_region) {
                hist_total->SetBinContent(bin, 0);
            }
        }
    }
    file->cd();
    hist_total->Write(("fit_" + mode).c_str());

}
