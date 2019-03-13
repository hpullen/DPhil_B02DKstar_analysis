#include "TFile.h"
#include "TStyle.h"
#include "TH1F.h"

#include "RooAddPdf.h"
#include "RooArgSet.h"
#include "RooDataHist.h"
#include "RooHist.h"
#include "RooPlot.h"
#include "RooAbsPdf.h"

#include "ShapeMakerBase.hpp"
#include "ToyBDTPdfMaker.hpp"
#include "ParameterManager.hpp"
#include "ShapeManager.hpp"

// ===========
// Constructor
// ===========
ShapeMakerBase::ShapeMakerBase(std::string name, RooRealVar * x, RooCategory * cat) : 
    m_name(name), 
    m_x(x),
    m_modes(MakeModeList(cat)),
    m_shapeMade(false),
    m_pdf(new RooSimultaneous((name + "_simPdf").c_str(), "", *cat)),
    m_cat(cat)
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
    std::cout << std::endl;
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


// ===============================
// Change the value of a parameter
// ===============================
void ShapeMakerBase::SetParameter(std::string name, double value, bool constant) {
    m_pars->ChangeValue(name, value);
    if (constant) m_pars->SetConstant(name);
}


// ============================
// Get value of a fit parameter
// ============================
double ShapeMakerBase::GetParameterValue(std::string name) {
    RooRealVar * var = (RooRealVar*)m_pars->Get(name);
    return var->getVal();
}


// ============================
// Get error of a fit parameter
// ============================
double ShapeMakerBase::GetParameterError(std::string name) {
    RooRealVar * var = (RooRealVar*)m_pars->Get(name);
    return var->getError();
}

// =============
// Get parameter
// =============
RooAbsReal * ShapeMakerBase::GetParameter(std::string name) {
    RooAbsReal * var = m_pars->Get(name);
    return var;
}


// ======================================
// Set maximum yields using a RooAbsData
// ======================================
void ShapeMakerBase::SetMaxYields(RooAbsData * data) {

    // Name of RooCategory
    std::string cat_name = m_cat->GetName();

    // Initialise yields to zero
    for (auto mode : m_modes) {
        std::string mode_short;
        for (std::string suffix : {"_plus", "_minus"}) {
            if (mode.find(suffix) != std::string::npos) {
                mode_short = mode.substr(0, mode.find(suffix)) + 
                    mode.substr(mode.find(suffix) + suffix.length(), 
                            std::string::npos);
            }
        }
        m_maxYields[mode_short] = 0;
    }

    // Loop through modes
    for (auto mode : m_modes) {
        RooAbsData * mode_data = data->reduce((cat_name + "==" + 
               cat_name + "::" + mode).c_str());

        // Check if mode is split into plus/minus
        bool split = false;
        std::string mode_short;
        for (std::string suffix : {"_plus", "_minus"}) {
            if (mode.find(suffix) != std::string::npos) {
                split = true;
                mode_short = mode.substr(0, mode.find(suffix)) + 
                    mode.substr(mode.find(suffix) + suffix.length(), 
                            std::string::npos);
            }
        }

        // Add to max yields
        if (!split) {
            m_maxYields[mode] = mode_data->sumEntries();
        } else {
            if (m_maxYields[mode_short] == m_defaultMaxYield) {
                m_maxYields[mode_short] = mode_data->sumEntries();
            } else {
                m_maxYields[mode_short] += mode_data->sumEntries();
            }
        }
    }

    // Set flag to indicate that shape should be remade before using
    m_shapeMade = false;
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


// =========================================
// Get integral of a shape wrt a total yield
// =========================================
std::pair<double, double> ShapeMakerBase::GetComponentIntegral(std::string shape, std::string yield, 
        double min, double max) {
    return GetComponentIntegral((RooAbsPdf*)m_shapes->Get(shape), 
            (RooRealVar*)m_pars->Get(yield), min, max);
}


// =========================================
// Get integral of a shape wrt a total yield
// =========================================
std::pair<double, double> ShapeMakerBase::GetComponentIntegral(RooAbsPdf * pdf, RooRealVar * yield, 
        double min, double max) {

    // Convert shape to a TH1F
    TH1F * hist = (TH1F*)pdf->createHistogram( "hist_to_integrate", *m_x, 
            RooFit::Binning(10 * m_x->getBins()));

    // Integrate in range
    double int_range = IntegrateInRange(hist, min, max);
    double error = 0;
    double int_total = hist->IntegralAndError(0, hist->GetNbinsX(), error);

    // Scale wrt yield
    double total_yield = yield->getVal();
    delete hist;
    double scaled_yield = total_yield * int_range / int_total;
    double scaled_err = error * int_range / int_total;
    return std::make_pair(scaled_yield, scaled_err);
}


// =============================================
// Get integral of fit shape (i.e. no rescaling)
// =============================================
std::pair<double, double> ShapeMakerBase::GetFitIntegral(std::string mode, double min, double max) {

    // Get components
    RooAddPdf * pdf = (RooAddPdf*)m_shapes->Get(mode);
    RooArgList * comps = new RooArgList(pdf->pdfList());
    RooArgList * coefs = new RooArgList(pdf->coefList());

    // Sum integrals
    double integral = 0;
    double sq_err = 0;
    for (int i = 0; i < comps->getSize(); i++) {
        std::pair<double, double> integ = GetComponentIntegral((RooAbsPdf*)comps->at(i), 
                (RooRealVar*)coefs->at(i), min, max);
        integral += integ.first;
        sq_err += pow(integ.second, 2);

    }
    return std::make_pair(integral, sqrt(sq_err));
}


// ============================================
// Integrate a histogram in a specified x range
// ============================================
double ShapeMakerBase::IntegrateInRange(TH1F * hist, double min, double max) {
    TAxis * axis = hist->GetXaxis();
    int bin_min = axis->FindBin(min);
    int bin_max = axis->FindBin(max);
    double integral = hist->Integral(bin_min, bin_max);
    integral -= hist->GetBinContent(bin_min) * (min - axis->GetBinLowEdge(bin_min))/
        axis->GetBinWidth(bin_min);
    integral -= hist->GetBinContent(bin_max) * (axis->GetBinUpEdge(bin_max) - max)/
        axis->GetBinWidth(bin_max);
    return integral;
}


// ==========================
// Print parameters to a file
// ==========================
void ShapeMakerBase::PrintToFile(std::string filename) {
    if (!m_shapeMade) {
        MakeShape();
    }
    m_pars->PrintValues(filename);
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

    std::cout << "In first histogram saving function" << std::endl;

    // Open file for saving
    TFile * outfile = TFile::Open(filename.c_str(), "RECREATE");

    // Save fit shapes
    std::map<std::string, int> n_bins;
    for (auto mode : m_modes) {
        n_bins[mode] = 80;
    }
    SaveFitShapes(outfile, blind, n_bins);

    // Save
    outfile->Close();
}


// ========================================
// Save histograms to a file including data
// ========================================
void ShapeMakerBase::SaveHistograms(std::string filename, RooAbsData * data, bool binned,
        bool blind) {

    // Check shape has been made
    if (!m_shapeMade) {
        std::cout << "Call to SaveHistograms before shape is created." << std::endl;
        std::cout << "Creating shape." << std::endl;
        MakeShape();
    }

    // Open file for saving
    TFile * outfile = TFile::Open(filename.c_str(), "RECREATE");

    // Number of bins for each mode
    std::map<std::string, int> n_bins;

    // Save data and pulls for each mode
    std::string cat_name = m_cat->GetName();
    for (auto mode : m_modes) {

        // Make data histogram
        RooDataHist * mode_data = (RooDataHist*)data->reduce((cat_name + "==" + 
               cat_name + "::" + mode).c_str());
        n_bins[mode] = 80;
        if (mode.find("KK") == 0 || mode.find("pipi") == 0) n_bins[mode] = 50;
        TH1F * mode_hist;
        if (binned) {
            mode_hist = (TH1F*)mode_data->createHistogram(("data_hist_" + 
                    mode).c_str(), *m_x);
            n_bins[mode] = mode_hist->GetXaxis()->GetNbins();
        } else {
            mode_hist = (TH1F*)mode_data->createHistogram(("data_hist_" + 
                    mode).c_str(), *m_x, RooFit::Binning(n_bins[mode]));
        }

        // Blind if needed
        const double B_mass = 5279.61;
        const double blind_region = 50;
        if (blind && ShouldBeBlind(mode)) {
            std::cout << "BLINDING HISTOGRAM" << std::endl;
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

        // Get pulls 
        RooPlot * frame = m_x->frame();
        mode_data->plotOn(frame, RooFit::Binning(n_bins[mode]));
        m_shapes->Get(mode)->plotOn(frame, RooFit::Binning(n_bins[mode]));
        RooHist * pulls = frame->pullHist();

        // Blind pulls if needed
        // if (blind && ShouldBeBlind(mode)) {
            // double x_val;
            // double y_val;
            // for (int i = 0; i < pulls->GetN(); i++) {
                // pulls->GetPoint(i, x_val, y_val);
                // if (abs(x_val - B_mass) < blind_region) {
                    // pulls->SetPoint(i, x_val, 0);
                // }
            // }
        // }

        pulls->Write(("pulls_" + mode).c_str());
    }

    // Save fit shapes
    std::cout << "Saving histograms for " << m_name << " to " << filename << 
        std::endl;
    SaveFitShapes(outfile, blind, n_bins);


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

    // Set up shapes
    MakeComponents();
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


// ====================================
// Return the maxiumum yield for a mode
// ====================================
double ShapeMakerBase::GetMaxYield(std::string mode) {

    // Return default value if not in map
    if (m_maxYields.find(mode) == m_maxYields.end()) {
        return m_defaultMaxYield;
    } else {
        return m_maxYields[mode];
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
void ShapeMakerBase::SaveFitShapes(TFile * file, bool blind, std::map<std::string, int> n_bins) {

    // Loop through modes and make histograms
    for (auto mode : m_modes) {
        SaveSingleFitShape(mode, file, n_bins[mode]);
    }
}


// ==================================
// Save a single fit shape for a mode
// ==================================
void ShapeMakerBase::SaveSingleFitShape(std::string mode, TFile * file, int n_bins) 
{

    // Parameters for blinding
    bool blind = false;
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
                    comp_name).c_str(), *m_x, RooFit::Binning(10 * n_bins));
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

                // Name without plus/minus
                std::string mode_short = another_mode;
                if (another_mode.find("_plus") != std::string::npos) {
                    mode_short = another_mode.substr(0, another_mode.find("_plus"));
                } else if (another_mode.find("_minus") != std::string::npos) {
                    mode_short = another_mode.substr(0, another_mode.find("_minus"));
                }
                if (mode_short.find("_run") != std::string::npos) {
                    mode_short = mode_short.substr(0, mode_short.find("_run"));
                }

                // Check for mode name
                std::size_t pos = comp_name.find("_" + mode_short);
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
            *m_x, RooFit::Binning(10 * n_bins));
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


// ==============================================
// Return whether or not a mode should be blinded
// ==============================================
bool ShapeMakerBase::ShouldBeBlind(std::string mode) {
    bool should_be_blind = true;
    if (mode.find("Kpi") == 0) should_be_blind = false;
    else if (mode.find("run1") != std::string::npos) should_be_blind = false;
    return should_be_blind;
}


// =====================
// Check for constraints
// =====================
bool ShapeMakerBase::HasConstraints() {
    return m_shapes->HasConstraints();
}


// ===============================
// Return RooArgset of constraints
// ===============================
RooArgSet * ShapeMakerBase::GetConstraintPdfs() {
    return m_shapes->GetConstraintPdfs();
}
