#include <string>

#include "TTree.h"
#include "TBox.h"

#include "RooAddPdf.h"
#include "RooFormulaVar.h"
#include "RooArgList.h"
#include "RooArgSet.h"
#include "RooCBShape.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooFitResult.h"
#include "RooPlot.h"
#include "RooGaussian.h"
#include "RooChebychev.h"
#include "RooRealVar.h"

#include "PlotStyle.hpp"

// ===============================================
// Function to fit and plot a D0 mass distribution
// ===============================================
double FitDmass(TTree * tree, std::string mode, std::string run, bool high_stats, bool with_cut, 
        std::string window_opt) {

    // D0 mass variable
    double mass_diff = (mode == "pipipipi") ? 80 : 100;
    RooRealVar D0_M("D0_M", "", 1864.84 - mass_diff, 1864.84 + mass_diff);
    double binWidth = 4;
    double nBins = ((D0_M.getMax() - D0_M.getMin())/binWidth);
    D0_M.setBins(nBins);
    RooRealVar D0_FDS("D0_FDS", "", 3, 999999);

    // Make RooDataSet
    RooArgList args(D0_M);
    if (with_cut) args.add(D0_FDS);
    RooDataSet * data = new RooDataSet("data", "", tree, args);

    // Fit parameters
    RooRealVar mean("mean", "", 1864.84, 1840, 1900);
    RooRealVar width("width", "", 10, 5, 20);
    RooRealVar ratio("ratio", "", 0.5, 0.2, 1);
    RooFormulaVar width2("width2", "@0 * @1", RooArgList(width, ratio));
    RooRealVar frac("frac", "", 0.5, 0, 1);
    RooRealVar a1("a1", "", 0, -1, 1);
    RooRealVar * f_gauss;
    if (mode == "KK" || mode == "pipi" || mode == "pipipipi") {
        f_gauss = new RooRealVar("f_gauss", "", 0.3, 0, 0.5);
    } else {
        f_gauss = new RooRealVar("f_gauss", "", 0.5, 0, 1);
    }
    
    // Make shapes
    RooGaussian gauss1("gauss1", "", D0_M, mean, width);
    RooGaussian gauss2("gauss2", "", D0_M, mean, width2);
    RooAbsPdf * gauss;
    if (mode == "pipi" || mode == "pipipipi") {
        gauss = new RooGaussian("gauss", "", D0_M, mean, width);
    } else {
        gauss = new RooAddPdf("gauss", "", RooArgList(gauss1, gauss2), 
                RooArgList(frac));
    }
    RooChebychev linear("linear", "", D0_M, RooArgList(a1));

    // Add a mis-ID shape for KK/pipi
    RooGaussian * misID;
    RooRealVar * misID_mean;
    RooRealVar misID_width("misID_width", "", 10, 0, 30);
    RooRealVar f_misID("f_misID", "", 0.2, 0, 0.5);
    RooAddPdf * D0_model;
    if (mode == "KK") {
        misID_mean = new RooRealVar("misID_mean", "", 1950, 1900, 2000);
    } else {
        misID_mean = new RooRealVar("misID_mean", "", 1790, 1750, 1820);
    }
    if (mode == "KK" || mode == "pipi") {
        misID = new RooGaussian("misID", "", D0_M, *misID_mean, misID_width);
        D0_model = new RooAddPdf("D0_model", "", RooArgList(*gauss, *misID, linear),
                RooArgList(*f_gauss, f_misID));
    } else {
        D0_model = new RooAddPdf("D0_model", "", RooArgList(*gauss, linear), 
                RooArgList(*f_gauss));
    }

    // Fit
    RooFitResult * r = D0_model->fitTo(*data, RooFit::Save(true), 
            RooFit::Offset(false), RooFit::Minimizer("Minuit2", "migrad"), 
            RooFit::Strategy(2));
    r->Print();

    // Plot fit results
    RooPlot * frame = D0_M.frame();
    data->plotOn(frame, RooFit::DrawOption("PZ"), RooFit::LineWidth(1), RooFit::MarkerStyle(8), RooFit::MarkerSize(1));
    D0_model->plotOn(frame, RooFit::Components("gauss"), RooFit::LineWidth(2),
            RooFit::ProjWData(*data), RooFit::LineColor(ANAGreen), 
            RooFit::DrawOption("C"));
    D0_model->plotOn(frame, RooFit::Components("linear"), RooFit::LineWidth(2),
            RooFit::ProjWData(*data), RooFit::LineColor(ANAPurple), 
            RooFit::DrawOption("C"));
    if (mode == "KK" || mode == "pipi") {
        D0_model->plotOn(frame, RooFit::Components("misID"), RooFit::LineWidth(2),
                RooFit::ProjWData(*data), RooFit::LineColor(kRed), 
                RooFit::DrawOption("C"));
    }
    D0_model->plotOn(frame,  RooFit::LineWidth(2), RooFit::ProjWData(*data), 
            RooFit::LineColor(kBlack), RooFit::DrawOption("C"));
    data->plotOn(frame, RooFit::DrawOption("PZ"), RooFit::LineWidth(1), RooFit::MarkerStyle(8), RooFit::MarkerSize(1));


    // Set axis titles
    TString latex_name;
    if (mode == "Kpi") latex_name = "K#pi";
    else if (mode == "piK") latex_name = "#pi K";
    else if (mode == "KK") latex_name = "KK";
    else if (mode == "pipi") latex_name = "#pi#pi";
    else if (mode == "Kpipipi") latex_name = "K#pi#pi#pi";
    else if (mode == "piKpipi") latex_name = "#piK#pi#pi";
    else if (mode == "pipipipi") latex_name = "#pi#pi#pi#pi";
    frame->GetXaxis()->SetTitle("#it{m}([" + latex_name + "]_{D}) [MeV/#it{c}^{2}]");
    frame->GetYaxis()->SetTitle("Candidates / (4 MeV/#it{c}^{2})");
    

    // Draw on canvas
    TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
    frame->Draw();

    // Draw boxes showing sideband regions
    TBox low_box(1864.84 - mass_diff, 0, 1864.84 - 50, frame->GetMaximum());
    TBox high_box(1864.84 + 50, 0, 1864.84 +  mass_diff, frame->GetMaximum());
    low_box.SetFillColorAlpha(ANABlue, 0.3);
    high_box.SetFillColorAlpha(ANABlue, 0.3);
    if (mode != "pipi" && mode != "pipipipipi") {
        low_box.Draw();
    }
    if (mode != "KK") {
        high_box.Draw();
    }
    gPad->RedrawAxis();

    // Save
    std::string dir = high_stats ? "/high_stats/" : "";
    std::string extra = with_cut ? "_withCut" : "";
    canvas->SaveAs(("Plots/" + dir + "D0_mass_fit_" + mode + "_run_" + run + extra + ".pdf").c_str());
    delete canvas;

    // Get ratio of sideband to mass window
    D0_M.setRange("low", 1864.84 - 100, 1864.84 - 50);
    D0_M.setRange("inner", 1864.84 - 25, 1864.84 + 25);
    D0_M.setRange("high", 1864.84 + 50, 1864.84 + 100);
    double int_low = linear.analyticalIntegral(1, "low");
    double int_inner = linear.analyticalIntegral(1, "inner");
    double int_high = linear.analyticalIntegral(1, "high");
    std::cout << "Low mass sideband: " << int_low << std::endl;
    std::cout << "Inner mass window: " << int_inner << std::endl;
    std::cout << "High mass sideband: " << int_high << std::endl;
    double total_sidebands;
    if (mode == "KK") total_sidebands = int_low;
    else if (mode == "pipi" || mode == "pipipipi") total_sidebands = int_high;
    else if (window_opt == "low") total_sidebands = int_low;
    else if (window_opt == "high") total_sidebands = int_high;
    else total_sidebands = int_low + int_high;
    return int_inner/total_sidebands;

}
