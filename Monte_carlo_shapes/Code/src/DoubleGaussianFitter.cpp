#include <fstream>
#include <iostream>

#include "TFile.h"

#include "RooAddPdf.h"
#include "RooDataHist.h"
#include "RooDataSet.h"
#include "RooGaussian.h"
#include "RooHist.h"
#include "RooPlot.h"
#include "RooRealVar.h"
#include "RooFitResult.h"

#include "DoubleGaussianFitter.hpp"
#include "RangeReader.hpp"

// ===========
// Constructor
// ===========
DoubleGaussianFitter::DoubleGaussianFitter(std::string name) : Fitter(name) {
    // Read in starting parameters
    std::string paramFile = "../Inputs/Starting_params/" + name + ".param";
    m_spr = new StartingParamReader(paramFile);
}

// ==========
// Destructor
// ==========
DoubleGaussianFitter::~DoubleGaussianFitter() {
    delete m_spr;
}

// ==========================================
// Perform a double Gaussian fit to histogram
// ==========================================
std::map<std::string, double> DoubleGaussianFitter::fit(TH1F* hist) {
    // Get min and max x values
    int xMin = hist->GetXaxis()->GetXmin();
    int xMax = hist->GetXaxis()->GetXmax();

    // Declare observable x
    RooRealVar x("x", "x", xMin, xMax);

    // Import histogram as RooDataHist
    RooDataHist dHist("dHist", "dHist", x, RooFit::Import(*hist));

    // Plot dataset
    RooPlot* frame1 = x.frame(RooFit::Title("Fit to monte carlo"));
    dHist.plotOn(frame1);

    // Make Gaussian variables
    // RooRealVar mean("mean", "", xMin, xMax);
    // RooRealVar width1("width1", "", width1_start, width1_low, width1_high);
    // RooRealVar widthRatio("widthRatio", "", widthRatio_start, width1_low,
           // widthRatio_high);
    // RooFormulaVar width2("width2", "", "@0*@1", RooArgList(widthRatio, width1));
    // RooRealVar frac1("frac1", "Coefficient of gauss1", frac1_start, frac1_low,
            // frac1_high);

    RooRealVar mean("mean", "", xMin, xMax);
    RooRealVar width1("width1", "", width1_start, width1_low, width1_high);
    RooRealVar widthRatio("widthRatio", "", widthRatio_start, width1_low, 
           widthRatio_high);
    RooFormulaVar width2("width2", "", "@0*@1", RooArgList(widthRatio, width1));
    RooRealVar frac1("frac1", "Coefficient of gauss1", frac1_start, frac1_low, 
            frac1_high);

    // Make Gaussians and add
    RooGaussian gauss1("gauss1", "", x, mean, width1);
    RooGaussian gauss2("gauss2", "", x, mean, width2);
    RooAddPdf doubleGaussian("doubleGaussian", "Sum of two Gaussians",
            RooArgList(gauss1, gauss2), RooArgList(frac1));

    // Fit to data
    doubleGaussian.fitTo(dHist, RooFit::Range(xMin, xMax), 
            RooFit::Offset(true), RooFit::Minimizer("Minuit2"));

    // Plot fit 
    doubleGaussian.plotOn(frame1);

    // Get pulls
    RooHist* pulls = frame1->pullHist();
    pulls->SetFillColor(kBlue + 3);

    // Print fit goodness
    std::cout << "Chi2: " << frame1->chiSquare() << std::endl;

    // Create TH1s from RooPdfs
    TH1F* hFit = (TH1F*)doubleGaussian.createHistogram("fit", x, 
            RooFit::Binning(1000));
    TH1F* hGauss1 = (TH1F*)gauss1.createHistogram("gauss1", x, 
            RooFit::Binning(1000));
    TH1F* hGauss2 = (TH1F*)gauss2.createHistogram("gauss2", x,
            RooFit::Binning(1000));

    // Scale histograms
    hFit->Scale(hist->Integral() * 10);
    hGauss1->Scale(hist->Integral() * 10 * frac1.getVal());
    hGauss2->Scale(hist->Integral() * 10 * (1 - frac1.getVal()));

    // Write histograms to root file 
    std::string outname = "../Histograms/" + this->m_name + "_fit.root";
    TFile* output = TFile::Open(outname.c_str(), "RECREATE");
    hist->Write("data");
    hFit->Write("fit");
    hGauss1->Write("gauss1");
    hGauss2->Write("gauss2");
    pulls->Write("pulls");
    output->Close();

    // Delete histograms
    delete hFit;
    delete hGauss1;
    delete hGauss2;

    // Make parameters map
    std::map<std::string, double> parameters;

    // Fill with parameters from fit
    parameters.emplace("mean", mean.getVal());
    parameters.emplace("sigma", width1.getVal());
    parameters.emplace("ratio", widthRatio.getVal());
    parameters.emplace("frac", frac1.getVal());

    // Plot the fit nicely
    plotFit("gauss1", "gauss2");

    // Return parameters map
    return parameters;
}


// =====================================
// Setup default fit starting parameters
// =====================================
void DoubleGaussianFitter::setupDefaultParams() {
    // width1
    width1_start = 20;
    width1_low = 0;
    width1_high = 200;

    // widthRatio
    widthRatio_start = 3;
    widthRatio_low = 0;
    widthRatio_high = 20;

    // frac1
    frac1_start = 0.88;
    frac1_low = 0;
    frac1_high = 1;
}


// ========================================
// Read starting fit parameters from a file
// ========================================
void DoubleGaussianFitter::readStartingParams(std::string paramFile) {
    // Open file
    std::ifstream file(paramFile);

    // Read parameters word-by-word
    std::string word;
    std::string params_read = "";

    // width1 
    file >> word;
    file >> word;
    if (!file.good()) {
        std::cout << "ERROR: Could not find parameters file: " << paramFile << std::endl;
        return;
    }
    width1_start = std::stod(word);
    file >> word;
    width1_low = std::stod(word);
    file >> word;
    width1_high = std::stod(word);
    params_read += "width1";

    // widthRatio
    file >> word;
    file >> word;
    widthRatio_start = std::stod(word);
    file >> word;
    widthRatio_low = std::stod(word);
    file >> word;
    widthRatio_high = std::stod(word);
    params_read += ", widthRatio";

    // frac1
    file >> word;
    file >> word;
    frac1_start = std::stod(word);
    file >> word;
    frac1_low = std::stod(word);
    file >> word;
    frac1_high = std::stod(word);
    params_read += ", frac1";
    std::cout << "Parameters read from file " << paramFile << ": " <<
        params_read << std::endl;

    // Close file
    file.close();
}
