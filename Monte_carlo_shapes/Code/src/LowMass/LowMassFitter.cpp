#include <fstream>
#include <iostream>

#include "TCanvas.h"
#include "TLegend.h"
#include "TFile.h"
#include "TH1F.h"
#include "TROOT.h"
#include "TLine.h"

#include "RooAbsPdf.h"
#include "RooAddPdf.h"
#include "RooArgList.h"
#include "RooCBShape.h"
#include "RooCategory.h"
#include "RooDataHist.h"
#include "RooDataSet.h"
#include "RooExponential.h"
#include "RooFitResult.h"
#include "RooFormulaVar.h"
#include "RooGaussian.h"
#include "RooHILLdini.h"
#include "RooHORNSdini.h"
#include "RooHist.h"
#include "RooLITTLEHORNSdini.h"
#include "RooMsgService.h"
#include "RooPlot.h"
#include "RooRealVar.h"
#include "RooSimultaneous.h"

#include "PlotStyle.hpp"
#include "LowMass/LowMassFitter.hpp"


// ===========
// Constructor
// ===========
LowMassFitter::LowMassFitter(std::string name, std::map<Component, 
        BackgroundComponent> components, int run_number) {
    m_name = name;
    m_components = components;
    m_run = run_number;
    processFit();
}


// ==========
// Destructor
// ==========
LowMassFitter::~LowMassFitter() {
}


// =====================
// Return fit parameters
// =====================
std::map<Component, AllParameters::LowMassOutput> LowMassFitter::getParameters() {
    return m_outputParams;
}


// ===============================================================
// Perform the fit process (read parameters, perform fit and plot)
// ===============================================================
void LowMassFitter::processFit() {

    // Read fit parameters
    readStartingParams();

    // Perform fit
    performFit();

    // Plot fit
    plotFits();
}


// =======================================
// Read starting fit parameters from files
// =======================================
void LowMassFitter::readStartingParams() {

    std::string filepath = "../Inputs/Starting_params/" + m_name + ".param";
    m_inputParams = AllParameters::readAllLowMassParameters(m_run);
}


// ============================
// Perform the simulatenous fit
// ============================
void LowMassFitter::performFit() {

    // Get histograms from component list
    TH1F * h_pi_100 = m_components[Component::pi_100].histogram;
    TH1F * h_pi_010 = m_components[Component::pi_010].histogram;
    TH1F * h_gamma_100 = m_components[Component::gamma_100].histogram;
    TH1F * h_gamma_010 = m_components[Component::gamma_010].histogram;
    TH1F * h_Bs_pi_100 = m_components[Component::Bs_pi_100].histogram;
    TH1F * h_Bs_pi_010 = m_components[Component::Bs_pi_010].histogram;
    TH1F * h_Bs_gamma_100 = m_components[Component::Bs_gamma_100].histogram;
    TH1F * h_Bs_gamma_010 = m_components[Component::Bs_gamma_010].histogram;

    // Declare observable x
    double xMin = h_pi_100->GetXaxis()->GetXmin();
    double xMax = h_pi_100->GetXaxis()->GetXmax();
    RooRealVar x("x", "x", xMin, xMax);

    // Import histograms as RooDataHists
    RooDataHist dHist_pi_100("dHist_pi_100", "", x, 
            RooFit::Import(*h_pi_100));
    RooDataHist dHist_pi_010("dHist_pi_010", "", x, 
            RooFit::Import(*h_pi_010));
    RooDataHist dHist_gamma_100("dHist_gamma_100", "", x, 
            RooFit::Import(*h_gamma_100));
    RooDataHist dHist_gamma_010("dHist_gamma_010", "", x, 
            RooFit::Import(*h_gamma_010));
    RooDataHist dHist_Bs_pi_100("dHist_Bs_pi_100", "", x, 
            RooFit::Import(*h_Bs_pi_100));
    RooDataHist dHist_Bs_pi_010("dHist_Bs_pi_010", "", x, 
            RooFit::Import(*h_Bs_pi_010));
    RooDataHist dHist_Bs_gamma_100("dHist_Bs_gamma_100", "", x, 
            RooFit::Import(*h_Bs_gamma_100));
    RooDataHist dHist_Bs_gamma_010("dHist_Bs_gamma_010", "", x, 
            RooFit::Import(*h_Bs_gamma_010));

    // Make variables
    RooRealVar shift("shift", "", 0);

    // Gamma 010
    RooRealVar a_gamma("a_gamma", "", 4919.1);
            // m_inputParams.gamma_010.a.start, m_inputParams.gamma_010.a.low, m_inputParams.gamma_010.a.high);
    RooRealVar b_gamma("b_gamma", "", 5225.5, 5222, 5228);
            // m_inputParams.gamma_010.b.start, m_inputParams.gamma_010.b.low, m_inputParams.gamma_010.b.high);
    RooRealVar csi_gamma_010("csi_gamma_010", "", m_inputParams.gamma_010.csi.start,
            m_inputParams.gamma_010.csi.low, m_inputParams.gamma_010.csi.high);
    RooRealVar sigma_gamma_010("sigma_gamma_010", "", 
            m_inputParams.gamma_010.sigma.start, m_inputParams.gamma_010.sigma.low, 
            m_inputParams.gamma_010.sigma.high);
    RooRealVar ratio("ratio", "", m_inputParams.gamma_010.ratio.start, 
            m_inputParams.gamma_010.ratio.low, m_inputParams.gamma_010.ratio.high);
    RooRealVar f("f", "", m_inputParams.gamma_010.f.start,
            m_inputParams.gamma_010.f.low, m_inputParams.gamma_010.f.high);

    // Gamma 100
    // RooRealVar a_gamma_100("a_gamma_100", "", m_inputParams.gamma_100.a.start,
            // m_inputParams.gamma_100.a.low, m_inputParams.gamma_100.a.high);
    // RooRealVar b_gamma_100("b_gamma_100", "", m_inputParams.gamma_100.b.start,
            // m_inputParams.gamma_100.b.low, m_inputParams.gamma_100.b.high);
    RooRealVar csi_gamma_100("csi_gamma_100", "", m_inputParams.gamma_100.csi.start,
            m_inputParams.gamma_100.csi.low, m_inputParams.gamma_100.csi.high);
    RooRealVar sigma_gamma_100("sigma_gamma_100", "", 
            m_inputParams.gamma_100.sigma.start, m_inputParams.gamma_100.sigma.low, 
            m_inputParams.gamma_100.sigma.high);
    // RooRealVar ratio_gamma_100("ratio_gamma_100", "",
            // m_inputParams.gamma_100.ratio.start, m_inputParams.gamma_100.ratio.low,
            // m_inputParams.gamma_100.ratio.high);
    // RooRealVar f_gamma_100("f_gamma_100", "", m_inputParams.gamma_100.f.start,
            // m_inputParams.gamma_100.f.low, m_inputParams.gamma_100.f.high);

    // Pi 010
    RooRealVar a_pi("a_pi", "", 5021.6, 5020, 5023);
            // m_inputParams.pi_010.a.start, m_inputParams.pi_010.a.low, m_inputParams.pi_010.a.high);
    RooRealVar b_pi("b_pi", "", 5116.9, 5112, 5118);
            // m_inputParams.pi_010.b.start, m_inputParams.pi_010.b.low, m_inputParams.pi_010.b.high);
    RooRealVar csi_pi_010("csi_pi_010", "", m_inputParams.pi_010.csi.start,
            m_inputParams.pi_010.csi.low, m_inputParams.pi_010.csi.high);
    RooRealVar sigma_pi_010("sigma_pi_010", "", 
            m_inputParams.pi_010.sigma.start, m_inputParams.pi_010.sigma.low, 
            m_inputParams.pi_010.sigma.high);
    // RooRealVar ratio_pi_010("ratio_pi_010", "",
            // m_inputParams.pi_010.ratio.start, m_inputParams.pi_010.ratio.low,
            // m_inputParams.pi_010.ratio.high);
    // RooRealVar f_pi_010("f_pi_010", "", m_inputParams.pi_010.f.start,
            // m_inputParams.pi_010.f.low, m_inputParams.pi_010.f.high);


    // Pi 100
    // RooRealVar a_pi_100("a_pi_100", "", m_inputParams.pi_100.a.start,
            // m_inputParams.pi_100.a.low, m_inputParams.pi_100.a.high);
    // RooRealVar b_pi_100("b_pi_100", "", m_inputParams.pi_100.b.start,
            // m_inputParams.pi_100.b.low, m_inputParams.pi_100.b.high);
    // RooRealVar csi_hill("csi_hill", "", m_inputParams.pi_100.csi.start,
            // m_inputParams.pi_100.csi.low, m_inputParams.pi_100.csi.high);
    RooRealVar csi_pi_100("csi_pi_100", "", m_inputParams.pi_100.csi.start,
            m_inputParams.pi_100.csi.low, m_inputParams.pi_100.csi.high);
    RooRealVar sigma_pi_100("sigma_pi_100", "", 
            m_inputParams.pi_100.sigma.start, m_inputParams.pi_100.sigma.low, 
            m_inputParams.pi_100.sigma.high);
    // RooRealVar ratio_pi_100("ratio_pi_100", "",
            // m_inputParams.pi_100.ratio.start, m_inputParams.pi_100.ratio.low,
            // m_inputParams.pi_100.ratio.high);
    // RooRealVar f_pi_100("f_pi_100", "", m_inputParams.pi_100.f.start,
            // m_inputParams.pi_100.f.low, m_inputParams.pi_100.f.high);

    // Make shapes
    RooHORNSdini pi_010("pi_010", "", x, a_pi, b_pi, csi_pi_010, shift, 
            sigma_pi_010, ratio, f);
    RooHILLdini gamma_010("gamma_010", "", x, a_gamma, b_gamma, 
            csi_gamma_010, shift, sigma_gamma_010, ratio, f);
    RooHILLdini pi_100("pi_100", "", x, a_pi, b_pi, csi_pi_100, shift, 
            sigma_pi_100, ratio, f);
    RooLITTLEHORNSdini gamma_100("gamma_100", "", x, a_gamma, b_gamma, 
            csi_gamma_100, shift, sigma_gamma_100, ratio, f, shift);

    // Endpoints for Bs shapes
    const double delta_M_val = 87.26;
    RooRealVar delta_M("delta_M", "", delta_M_val, delta_M_val - 5, 
            delta_M_val + 5);
    RooFormulaVar Bs_a_gamma("Bs_a_gamma", "", "@0 + @1", RooArgList(a_gamma, 
                delta_M));
    RooFormulaVar Bs_b_gamma("Bs_b_gamma", "", "@0 + @1", RooArgList(b_gamma, 
                delta_M));
    RooFormulaVar Bs_a_pi("Bs_a_pi", "", "@0 + @1", RooArgList(a_pi, 
                delta_M));
    RooFormulaVar Bs_b_pi("Bs_b_pi", "", "@0 + @1", RooArgList(b_pi, 
                delta_M));

    // Widths for Bs shapes
    RooRealVar Bs_sigma_pi_100("Bs_sigma_pi_100", "", 
            m_inputParams.pi_100.sigma.start, m_inputParams.pi_100.sigma.low, 
            m_inputParams.pi_100.sigma.high);
    RooRealVar Bs_sigma_pi_010("Bs_sigma_pi_010", "", 
            m_inputParams.pi_010.sigma.start, m_inputParams.pi_010.sigma.low, 
            m_inputParams.pi_010.sigma.high);
    RooRealVar Bs_sigma_gamma_100("Bs_sigma_gamma_100", "", 
            m_inputParams.gamma_100.sigma.start, m_inputParams.gamma_100.sigma.low, 
            m_inputParams.gamma_100.sigma.high);
    RooRealVar Bs_sigma_gamma_010("Bs_sigma_gamma_010", "", 
            m_inputParams.gamma_010.sigma.start, m_inputParams.gamma_010.sigma.low, 
            m_inputParams.gamma_010.sigma.high);

    // Csi for Bs shapes
    RooRealVar Bs_csi_pi_100("Bs_csi_pi_100", "", m_inputParams.pi_100.csi.start,
            m_inputParams.pi_100.csi.low, m_inputParams.pi_100.csi.high);
    RooRealVar Bs_csi_pi_010("Bs_csi_pi_010", "", m_inputParams.pi_010.csi.start,
            m_inputParams.pi_010.csi.low, m_inputParams.pi_010.csi.high);
    RooRealVar Bs_csi_gamma_100("Bs_csi_gamma_100", "", m_inputParams.gamma_100.csi.start,
            m_inputParams.gamma_100.csi.low, m_inputParams.gamma_100.csi.high);
    RooRealVar Bs_csi_gamma_010("Bs_csi_gamma_010", "", m_inputParams.gamma_010.csi.start,
            m_inputParams.gamma_010.csi.low, m_inputParams.gamma_010.csi.high);

    // Fraction and ratio for Bs shapes
    RooRealVar Bs_ratio("Bs_ratio", "", m_inputParams.gamma_010.ratio.start, 
            m_inputParams.gamma_010.ratio.low, m_inputParams.gamma_010.ratio.high);
    RooRealVar Bs_f("Bs_f", "", m_inputParams.gamma_010.f.start,
            m_inputParams.gamma_010.f.low, m_inputParams.gamma_010.f.high);

    // Make Bs shapes
    RooHORNSdini Bs_pi_010("Bs_pi_010", "", x, Bs_a_pi, Bs_b_pi, Bs_csi_pi_010, shift,
            Bs_sigma_pi_010, ratio, f);
    RooHILLdini Bs_pi_100("Bs_pi_100", "", x, Bs_a_pi, Bs_b_pi, Bs_csi_pi_100, shift,
            Bs_sigma_pi_100, ratio, f);
    RooHILLdini Bs_gamma_010("Bs_gamma_010", "", x, Bs_a_gamma, Bs_b_gamma, 
            Bs_csi_gamma_010, shift, Bs_sigma_gamma_010, ratio, f);
    RooLITTLEHORNSdini Bs_gamma_100("Bs_gamma_100", "", x, Bs_a_gamma, Bs_b_gamma, 
            Bs_csi_gamma_100, shift, Bs_sigma_gamma_100, ratio, f, shift);

    // Create categories
    RooCategory category("category", "");
    category.defineType("pi_010");
    category.defineType("pi_100");
    category.defineType("gamma_010");
    category.defineType("gamma_100");
    category.defineType("Bs_pi_010");
    category.defineType("Bs_pi_100");
    category.defineType("Bs_gamma_010");
    category.defineType("Bs_gamma_100");

    // Make map of datahists
    std::map<std::string, RooDataHist*> histMap;
    histMap.emplace("pi_010", &dHist_pi_010);
    histMap.emplace("pi_100", &dHist_pi_100);
    histMap.emplace("gamma_010", &dHist_gamma_010);
    histMap.emplace("gamma_100", &dHist_gamma_100);
    histMap.emplace("Bs_pi_010", &dHist_Bs_pi_010);
    histMap.emplace("Bs_pi_100", &dHist_Bs_pi_100);
    histMap.emplace("Bs_gamma_010", &dHist_Bs_gamma_010);
    histMap.emplace("Bs_gamma_100", &dHist_Bs_gamma_100);

    // Make combined dataset
    RooDataHist combData("combData", "", x, category, histMap);

    // Make simultaneous fit
    RooSimultaneous simFit("simFit", "", category);
    simFit.addPdf(pi_010, "pi_010");
    simFit.addPdf(pi_100, "pi_100");
    simFit.addPdf(gamma_010, "gamma_010");
    simFit.addPdf(gamma_100, "gamma_100");
    simFit.addPdf(Bs_pi_010, "Bs_pi_010");
    simFit.addPdf(Bs_pi_100, "Bs_pi_100");
    simFit.addPdf(Bs_gamma_010, "Bs_gamma_010");
    simFit.addPdf(Bs_gamma_100, "Bs_gamma_100");

    // Peform the fit
    RooFitResult * result = simFit.fitTo(combData, RooFit::Offset(true),
            RooFit::Minimizer("Minuit2"), RooFit::Save(true));
    result->Print("v");
    delete result;

    // Get pulls
    RooHist * pulls_gamma_010 = getPulls(&x, &dHist_gamma_010, &gamma_010);
    RooHist * pulls_gamma_100 = getPulls(&x, &dHist_gamma_100, &gamma_100);
    RooHist * pulls_pi_010 = getPulls(&x, &dHist_pi_010, &pi_010);
    RooHist * pulls_pi_100 = getPulls(&x, &dHist_pi_100, &pi_100);
    RooHist * pulls_Bs_gamma_010 = getPulls(&x, &dHist_Bs_gamma_010, &Bs_gamma_010);
    RooHist * pulls_Bs_gamma_100 = getPulls(&x, &dHist_Bs_gamma_100, &Bs_gamma_100);
    RooHist * pulls_Bs_pi_010 = getPulls(&x, &dHist_Bs_pi_010, &Bs_pi_010);
    RooHist * pulls_Bs_pi_100 = getPulls(&x, &dHist_Bs_pi_100, &Bs_pi_100);

    // Create TH1Fs from RooRdfs
    TH1F * hFit_Gamma_010 = (TH1F*)gamma_010.createHistogram("gamma_010", x,
            RooFit::Binning(1000));
    TH1F * hFit_Gamma_100 = (TH1F*)gamma_100.createHistogram("gamma_100", x,
            RooFit::Binning(1000));
    TH1F * hFit_Pi_010 = (TH1F*)pi_010.createHistogram("pi_010", x,
            RooFit::Binning(1000));
    TH1F * hFit_Pi_100 = (TH1F*)pi_100.createHistogram("pi_100", x,
            RooFit::Binning(1000));
    TH1F * hFit_Bs_Gamma_010 = (TH1F*)Bs_gamma_010.createHistogram("Bs_gamma_010",
            x, RooFit::Binning(1000));
    TH1F * hFit_Bs_Gamma_100 = (TH1F*)Bs_gamma_100.createHistogram("Bs_gamma_100",
            x, RooFit::Binning(1000));
    TH1F * hFit_Bs_Pi_010 = (TH1F*)Bs_pi_010.createHistogram("Bs_pi_010", x,
            RooFit::Binning(1000));
    TH1F * hFit_Bs_Pi_100 = (TH1F*)Bs_pi_100.createHistogram("Bs_pi_100", x,
            RooFit::Binning(1000));

    // Scale histograms
    hFit_Gamma_010->Scale(h_gamma_010->Integral() * 10);
    hFit_Gamma_100->Scale(h_gamma_100->Integral() * 10);
    hFit_Pi_010->Scale(h_pi_010->Integral() * 10);
    hFit_Pi_100->Scale(h_pi_100->Integral() * 10);
    hFit_Bs_Gamma_010->Scale(h_Bs_gamma_010->Integral() * 10);
    hFit_Bs_Gamma_100->Scale(h_Bs_gamma_100->Integral() * 10);
    hFit_Bs_Pi_010->Scale(h_Bs_pi_010->Integral() * 10);
    hFit_Bs_Pi_100->Scale(h_Bs_pi_100->Integral() * 10);

    // Write histograms to output file
    TFile * outfile = TFile::Open(("../Histograms/LowMass/" + m_name +
                ".root").c_str(), "RECREATE");
    // Data
    h_gamma_010->Write("gamma_010_data");
    h_gamma_100->Write("gamma_100_data");
    h_pi_010->Write("pi_010_data");
    h_pi_100->Write("pi_100_data");
    h_Bs_gamma_010->Write("Bs_gamma_010_data");
    h_Bs_gamma_100->Write("Bs_gamma_100_data");
    h_Bs_pi_010->Write("Bs_pi_010_data");
    h_Bs_pi_100->Write("Bs_pi_100_data");
    // Fit
    hFit_Gamma_010->Write("gamma_010_fit");
    hFit_Gamma_100->Write("gamma_100_fit");
    hFit_Pi_010->Write("pi_010_fit");
    hFit_Pi_100->Write("pi_100_fit");
    hFit_Bs_Gamma_010->Write("Bs_gamma_010_fit");
    hFit_Bs_Gamma_100->Write("Bs_gamma_100_fit");
    hFit_Bs_Pi_010->Write("Bs_pi_010_fit");
    hFit_Bs_Pi_100->Write("Bs_pi_100_fit");
    // Pulls
    pulls_gamma_010->Write("gamma_010_pulls");
    pulls_gamma_100->Write("gamma_100_pulls");
    pulls_pi_010->Write("pi_010_pulls");
    pulls_pi_100->Write("pi_100_pulls");
    pulls_Bs_gamma_010->Write("Bs_gamma_010_pulls");
    pulls_Bs_gamma_100->Write("Bs_gamma_100_pulls");
    pulls_Bs_pi_010->Write("Bs_pi_010_pulls");
    pulls_Bs_pi_100->Write("Bs_pi_100_pulls");
    outfile->Close();

    // Set output parameters
    // Gamma 010
    m_outputParams[Component::gamma_010].a = a_gamma.getVal();
    m_outputParams[Component::gamma_010].b = b_gamma.getVal();
    m_outputParams[Component::gamma_010].csi = csi_gamma_010.getVal();
    m_outputParams[Component::gamma_010].sigma = sigma_gamma_010.getVal();
    m_outputParams[Component::gamma_010].ratio = ratio.getVal();
    m_outputParams[Component::gamma_010].f = f.getVal();
    m_outputParams[Component::Bs_gamma_010].a = Bs_a_gamma.getVal();
    m_outputParams[Component::Bs_gamma_010].b = Bs_b_gamma.getVal();
    m_outputParams[Component::Bs_gamma_010].csi = Bs_csi_gamma_010.getVal();
    m_outputParams[Component::Bs_gamma_010].sigma = Bs_sigma_gamma_010.getVal();
    m_outputParams[Component::Bs_gamma_010].ratio = ratio.getVal();
    m_outputParams[Component::Bs_gamma_010].f = f.getVal();

    // Gamma 100
    m_outputParams[Component::gamma_100].a = a_gamma.getVal();
    m_outputParams[Component::gamma_100].b = b_gamma.getVal();
    m_outputParams[Component::gamma_100].csi = csi_gamma_100.getVal();
    m_outputParams[Component::gamma_100].sigma = sigma_gamma_100.getVal();
    m_outputParams[Component::gamma_100].ratio = ratio.getVal();
    m_outputParams[Component::gamma_100].f = f.getVal();
    m_outputParams[Component::Bs_gamma_100].a = Bs_a_gamma.getVal();
    m_outputParams[Component::Bs_gamma_100].b = Bs_b_gamma.getVal();
    m_outputParams[Component::Bs_gamma_100].csi = Bs_csi_gamma_100.getVal();
    m_outputParams[Component::Bs_gamma_100].sigma = Bs_sigma_gamma_100.getVal();
    m_outputParams[Component::Bs_gamma_100].ratio = ratio.getVal();
    m_outputParams[Component::Bs_gamma_100].f = f.getVal();

    // Pi 010
    m_outputParams[Component::pi_010].a = a_pi.getVal();
    m_outputParams[Component::pi_010].b = b_pi.getVal();
    m_outputParams[Component::pi_010].csi = csi_pi_010.getVal();
    m_outputParams[Component::pi_010].sigma = sigma_pi_010.getVal();
    m_outputParams[Component::pi_010].ratio = ratio.getVal();
    m_outputParams[Component::pi_010].f = f.getVal();
    m_outputParams[Component::Bs_pi_010].a = Bs_a_pi.getVal();
    m_outputParams[Component::Bs_pi_010].b = Bs_b_pi.getVal();
    m_outputParams[Component::Bs_pi_010].csi = Bs_csi_pi_010.getVal();
    m_outputParams[Component::Bs_pi_010].sigma = Bs_sigma_pi_010.getVal();
    m_outputParams[Component::Bs_pi_010].ratio = ratio.getVal();
    m_outputParams[Component::Bs_pi_010].f = f.getVal();

    // Pi 100
    m_outputParams[Component::pi_100].a = a_pi.getVal();
    m_outputParams[Component::pi_100].b = b_pi.getVal();
    m_outputParams[Component::pi_100].csi = csi_pi_100.getVal();
    m_outputParams[Component::pi_100].sigma = sigma_pi_100.getVal();
    m_outputParams[Component::pi_100].ratio = ratio.getVal();
    m_outputParams[Component::pi_100].f = f.getVal();
    m_outputParams[Component::Bs_pi_100].a = Bs_a_pi.getVal();
    m_outputParams[Component::Bs_pi_100].b = Bs_b_pi.getVal();
    m_outputParams[Component::Bs_pi_100].csi = Bs_csi_pi_100.getVal();
    m_outputParams[Component::Bs_pi_100].sigma = Bs_sigma_pi_100.getVal();
    m_outputParams[Component::Bs_pi_100].ratio = ratio.getVal();
    m_outputParams[Component::Bs_pi_100].f = f.getVal();

}


// ===================================
// Get pulls histogram for a component
// ===================================
RooHist * LowMassFitter::getPulls(RooRealVar * x, RooDataHist * hData, 
        RooAbsPdf * hFit) {
    RooPlot * frame = x->frame();
    hData->plotOn(frame);
    hFit->plotOn(frame);
    RooHist * pulls = frame->pullHist();
    pulls->SetFillColor(kBlue + 3);
    return pulls;
}


// ====================
// Plot the fit results
// ====================
void LowMassFitter::plotFits() {

    // Plot each fit
    plotSingleFit(Component::gamma_010);
    plotSingleFit(Component::gamma_100);
    plotSingleFit(Component::pi_010);
    plotSingleFit(Component::pi_100);
    plotSingleFit(Component::Bs_gamma_010);
    plotSingleFit(Component::Bs_gamma_100);
    plotSingleFit(Component::Bs_pi_010);
    plotSingleFit(Component::Bs_pi_100);
    

}


// =================
// Plot a single fit
// =================
void LowMassFitter::plotSingleFit(Component comp) {

    // Set custom plotting style
    setPlotStyle();

    // Get histogram file
    std::string filename = "../Histograms/LowMass/" + m_name + ".root";
    TFile * file = TFile::Open(filename.c_str(), "READ");
    gROOT->ForceStyle();

    // Get histograms
    std::string comp_name;
    switch (comp) {
        case Component::gamma_010: comp_name = "gamma_010";
                                   break;
        case Component::gamma_100: comp_name = "gamma_100";
                                   break;
        case Component::pi_010: comp_name = "pi_010";
                                   break;
        case Component::pi_100: comp_name = "pi_100";
                                   break;
        case Component::Bs_gamma_010: comp_name = "Bs_gamma_010";
                                   break;
        case Component::Bs_gamma_100: comp_name = "Bs_gamma_100";
                                   break;
        case Component::Bs_pi_010: comp_name = "Bs_pi_010";
                                   break;
        case Component::Bs_pi_100: comp_name = "Bs_pi_100";
                                   break;
    }
    TH1F * hData = (TH1F*)file->Get((comp_name + "_data").c_str());
    TH1F * hFit = (TH1F*)file->Get((comp_name + "_fit").c_str());
    RooHist * hPull = (RooHist*)file->Get((comp_name + "_pulls").c_str());

    // Make canvas
    TCanvas* canvas = new TCanvas("Bd_M", "", 1.25 * 700, 1000);
    TPad* pad1 = new TPad("Fit", "", 0.0, 0.3, 1.0, 1.0);
    pad1->cd();

    // Draw data points
    hData->SetXTitle("M([K#pi]_{D}K^{*0})[MeV/#it{c}^{2}]");
    hData->GetYaxis()->SetTitle("Candidates / (6 MeV/#it{c}^{2})");
    hData->SetLineColor(kBlack);
    hData->SetLineWidth(1);
    hData->SetMarkerSize(0);
    hData->SetStats(kFALSE);
    hData->GetXaxis()->SetRangeUser(4809, 5399);
    hData->GetXaxis()->SetLabelSize(0.06);
    hData->GetXaxis()->SetTitleOffset(1.1);
    hData->GetYaxis()->SetLabelSize(0.06);
    hData->Draw("E");

    // Draw fit
    hFit->SetLineColor(ANABlue);
    hFit->SetMarkerColor(ANABlue);
    hFit->SetMarkerStyle(0);
    hFit->Draw("C SAME");
    hData->Draw("E SAME");
    
    // Add legend
    TLegend* leg = new TLegend(0.2, 0.75, 0.5, 0.9);
    leg->AddEntry(hData, "Monte Carlo");
    leg->AddEntry(hFit, "Fit");
    leg->SetFillStyle(0);
    leg->Draw();

    // Draw pulls
    TPad * pad2 = new TPad("Pulls", "", 0.0, 0.0, 1.0, 0.3);
    pad2->cd();
    RooPlot * frame = new RooPlot(hData->GetXaxis()->GetXmin(), 
            hData->GetXaxis()->GetXmax());
    frame->SetMinimum(-5);
    frame->SetMaximum(5);
    hPull->SetFillColor(kBlue + 3);
    frame->addPlotable(hPull, "BEX0");
    frame->Draw();
    TLine * line = new TLine(4800, -3, 5400, -3);
    line->SetLineStyle(2);
    line->SetLineColor(kRed + 2);
    line->Draw();
    TLine * line2 = new TLine(4800, 3, 5400, 3);
    line2->SetLineStyle(2);
    line2->SetLineColor(kRed + 2);
    line2->Draw();

    // Save
    canvas->cd();
    pad1->Draw();
    pad2->Draw();
    std::string outname = "../Plots/LowMass/" + m_name + "_" + comp_name + ".pdf";
    canvas->SaveAs(outname.c_str());
    delete hData;
    delete hFit;
    delete canvas;
}

