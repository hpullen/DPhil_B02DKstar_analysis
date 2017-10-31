#include <fstream>
#include <iostream>

#include "TCanvas.h"
#include "TFile.h"

#include "RooAddPdf.h"
#include "RooCBShape.h"
#include "RooDataHist.h"
#include "RooDataSet.h"
#include "RooHist.h"
#include "RooPlot.h"
#include "RooRealVar.h"
#include "RooFitResult.h"

#include "DoubleCBFitter.hpp"

// ===========
// Constructor
// ===========
DoubleCBFitter::DoubleCBFitter(std::string name) : Fitter(name) {
}

// ==========
// Destructor
// ==========
DoubleCBFitter::~DoubleCBFitter() {
}

// ==============================================
// Perform a double Crystal Ball fit to histogram
// ==============================================
std::map<std::string, double> DoubleCBFitter::fit(TH1F* hist) {
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

    // // Make variables for crystal ball
    // RooRealVar mean("mean", "mean", mean_start, mean_low, mean_high);
    // RooRealVar sigma1("sigma1", "sigma1", sigma1_low, sigma1_high);
    // RooRealVar ratio("ratio", "ratio", 3, 0.1, 10);
    // RooRealVar alpha1("alpha1", "alpha1", alpha1_start, alpha1_low, alpha1_high);
    // RooRealVar alpha2("alpha2", "alpha2", alpha2_start, alpha2_low, alpha2_high);
    // std::cout << "Alpha2 values: " << alpha2_start << " " << alpha2_low << " " <<
        // alpha2_high << std::endl;
    // RooRealVar n("n", "n", 1);
    // RooRealVar frac1("frac1", "Fraction of CB1", frac1_start, frac1_low,
            // frac1_high);
    // RooFormulaVar sigma2("sigma1", "", "@0*@1", RooArgList(ratio, sigma1));

    RooRealVar mean("mean", "", mean_start, mean_low, mean_high);
    RooRealVar sigma1("sigma1", "", sigma1_start, sigma1_low, sigma1_high);
    RooRealVar sigmaRatio("sigmaRatio", "", 0.5, 0, 1);
    RooFormulaVar sigma2("sigma2", "", "@0*@1", RooArgList(sigmaRatio, sigma1));
    RooRealVar alpha1("alpha1", "alpha1", alpha1_start, alpha1_low, alpha1_high);
    RooRealVar alpha2("alpha2", "alpha2", alpha2_start, alpha2_low, alpha2_high);
    RooRealVar n("n", "n", 1);
    RooRealVar frac1("frac1", "Coefficient of CB1", frac1_start, frac1_low, 
            frac1_high);

    // Make crystal balls and add
    RooCBShape CB1("CB1", "", x, mean, sigma1, alpha1, n);
    RooCBShape CB2("CB2", "", x, mean, sigma2, alpha2, n);
    RooAddPdf doubleCB("doubleCB", "", RooArgList(CB1, CB2), RooArgList(frac1));

    // Fit to data
    doubleCB.fitTo(dHist, RooFit::Range(xMin, xMax), RooFit::Offset(true),
            RooFit::Minimizer("Minuit2"));

    // Plot fit 
    doubleCB.plotOn(frame1);

    // Get pulls
    RooHist* pulls = frame1->pullHist();
    pulls->SetFillColor(kBlue + 3);
    std::cout << "Chi2: " << frame1->chiSquare() << std::endl;

    // Create TH1s from RooPdfs
    TH1F* hFit = (TH1F*)doubleCB.createHistogram("fit", x, 
            RooFit::Binning(1000));
    TH1F* hCB1 = (TH1F*)CB1.createHistogram("CB1", x, 
            RooFit::Binning(1000));
    TH1F* hCB2 = (TH1F*)CB2.createHistogram("CB2", x,
            RooFit::Binning(1000));

    // Scale histograms
    hFit->Scale(hist->Integral() * 10);
    hCB1->Scale(hist->Integral() * 10 * frac1.getVal());
    hCB2->Scale(hist->Integral() * 10 * (1 - frac1.getVal()));

    // Write histograms to root file 
    std::string outname = "../Histograms/" + this->m_name + "_fit.root";
    TFile* output = TFile::Open(outname.c_str(), "RECREATE");
    hist->Write("data");
    hFit->Write("fit");
    hCB1->Write("CB1");
    hCB2->Write("CB2");
    pulls->Write("pulls");
    output->Close();

    // Delete histograms
    delete hFit;
    delete hCB1;
    delete hCB2;

    // Make parameters map
    std::map<std::string, double> parameters;

    // Fill with parameters from fit
    parameters.emplace("mean", mean.getVal());
    parameters.emplace("sigma1", sigma1.getVal());
    parameters.emplace("sigmaRatio", sigmaRatio.getVal());
    parameters.emplace("alpha1", alpha1.getVal());
    parameters.emplace("alpha2", alpha2.getVal());
    parameters.emplace("frac", frac1.getVal());
    parameters.emplace("n", n.getVal());

    // Plot the fit nicely
    plotFit("CB1", "CB2");

    // Return parameters map
    return parameters;
}


// ================================
// Setup default fitting parameters
// ================================
void DoubleCBFitter::setupDefaultParams() {
    // mean
    mean_start = 5320;
    mean_low = 5200;
    mean_high = 5500;

    // sigma1
    sigma1_low = 3;
    sigma1_high = 50;

    // alpha1
    alpha1_start = -2.0;
    alpha1_low = -10;
    alpha1_high = 0.0;

    // alpha2
    alpha2_start = 2.0;
    alpha2_low = 0.0;
    alpha2_high = 10;

    // frac1
    frac1_start = 0.5;
    frac1_low = 0;
    frac1_high = 1;
}
    
// ========================================
// Read starting fit parameters from a file
// ========================================
void DoubleCBFitter::readStartingParams(std::string paramFile) {
    // Open file
    std::ifstream file(paramFile);

    // Read parameters word-by-word
    std::string word;
    std::string params_read = "";

    // Check for file
    if (!file.good()) {
        std::cout << "No parameters file found at: " << paramFile << std::endl;
        return;
    }


    // mean 
    file >> word;
    file >> word;
    mean_start = std::stod(word);
    file >> word;
    mean_low = std::stod(word);
    file >> word;
    mean_high = std::stod(word);
    params_read += "mean";

    // sigma1
    file >> word;
    file >> word;
    sigma1_start = std::stod(word);
    file >> word;
    sigma1_low = std::stod(word);
    file >> word;
    sigma1_high = std::stod(word);
    params_read += ", sigma1";

    // alpha1
    file >> word;
    file >> word;
    alpha1_start = std::stod(word);
    file >> word;
    alpha1_low = std::stod(word);
    file >> word;
    alpha1_high = std::stod(word);
    params_read += ", alpha1";

    // alpha2
    file >> word;
    file >> word;
    alpha2_start = std::stod(word);
    file >> word;
    alpha2_low = std::stod(word);
    file >> word;
    alpha2_high = std::stod(word);
    params_read += ", alpha2";

    // frac1
    std::cout << "Reading frac1" << std::endl;
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
