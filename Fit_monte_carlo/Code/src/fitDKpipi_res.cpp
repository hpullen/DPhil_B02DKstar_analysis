#include <fstream>
#include <iostream>

#include "TApplication.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TFile.h"
#include "TH1F.h"
#include "TLegend.h"
#include "TString.h"

#include "RooAddPdf.h"
#include "RooArgList.h"
#include "RooArgSet.h"
#include "RooCategory.h"
#include "RooCBShape.h"
#include "RooGaussian.h"
#include "RooDataHist.h"
#include "RooDataSet.h"
#include "RooFitResult.h"
#include "RooFormulaVar.h"
#include "RooHILLdini.h"
#include "RooHist.h"
#include "RooHORNSdini.h"
#include "RooPlotable.h"
#include "RooPlot.h"
#include "RooRealVar.h"
#include "RooSimultaneous.h"

#include "PlotStyle.hpp"

// ==========================================
// Fit to B+ -> D0 K*0 pi+ shape with RooHILL
// ==========================================
int main(int argc, char * argv[]) {

    // Get data
    TFile * file = TFile::Open("/data/lhcb/users/pullen/RapidSim/B_DKpipi_study/"
            "with_evtGen/3_K1_1400_tree.root", "READ");
    TTree * tree = (TTree*)file->Get("DecayTree");

    // Cut on daughter masses
    TString cut = "(abs(D0_M - 1.86484) < 0.025 && abs(Kstar_M - 0.8955) < 0.05)";

    // Make RooDataSet
    double min = 5000;
    double max = 5500;
    RooRealVar * Bd_M = new RooRealVar("Bd_M", "", min, max);
    TH1F * hist = new TH1F("hist", "", 200, min, max);
    tree->Draw("Bd_M*1000>>hist", cut);
    RooDataHist * data = new RooDataHist("data", "", *Bd_M, RooFit::Import(*hist));

    // Make RooHILLdini
    RooRealVar a("a", "", 4700, 5000, 4400);
    RooRealVar b("b", "", 5139.69);
    // RooRealVar csi("csi", "", 0, -1, 1);
    RooRealVar csi("csi", "", 1);
    RooRealVar sigma("sigma", "", 5, 0, 100);
    RooRealVar ratio("ratio", "", 5, 1, 15);
    RooRealVar frac("frac", "", 0.5, 0, 1);
    RooRealVar shift("shift", "", 0);
    RooHILLdini hill("hill", "", *Bd_M, a, b, csi, shift, sigma, ratio, frac);

    // Make Gaussian
    RooRealVar sigma_gauss("sigma_gauss", "", 50, 0, 200);
    RooRealVar sigma_gauss2("sigma_gauss2", "", 50, 0, 200);
    // RooFormulaVar mean_gauss("mean_gauss", "(@0 + @1)/2", RooArgList(a, b));
    RooRealVar mean_gauss("mean_gauss", "", 5000, 4700, 5100);
    RooGaussian gauss("gauss", "", *Bd_M, mean_gauss, sigma_gauss);
    RooRealVar f_gauss("f_gauss", "", 0.2, 0, 1);
    RooAddPdf model("model", "", RooArgList(gauss, hill), RooArgList(f_gauss));

    // Perform fit
    RooFitResult * r = model.fitTo(*data, RooFit::Save(), 
            RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    r->Print("v");

    // Plot
    RooPlot * plt = Bd_M->frame();
    data->plotOn(plt, RooFit::MarkerStyle(1), RooFit::DrawOption("E"));
    model.plotOn(plt, RooFit::LineColor(kBlack));
    RooHist * pulls = plt->pullHist();
    model.plotOn(plt, RooFit::Components("gauss"), RooFit::LineColor(kRed));
    model.plotOn(plt, RooFit::Components("hill"), RooFit::LineColor(kBlue));
    TCanvas * canvas = new TCanvas("canvas", "", 500, 1000);
    canvas->Divide(1, 2);
    canvas->cd(1);
    plt->Draw();
    canvas->cd(2);
    RooPlot * plt2 = Bd_M->frame();
    plt2->addPlotable(pulls, "BEX0");
    plt2->Draw();
    canvas->SaveAs("../Plots/DKpipi_3.pdf");

    a.Print();
    b.Print();
    mean_gauss.Print();








}
