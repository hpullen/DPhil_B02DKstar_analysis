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
            "with_evtGen/2_Kstar_tree.root", "READ");
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
    RooRealVar a("a", "", 2760.65);
    RooRealVar b("b", "", 5139.69);/* , 5130, 5160); */
    RooRealVar csi("csi", "", -5, -9, 0.0);
    RooRealVar sigma("sigma", "", 5, 0, 100);
    RooRealVar ratio("ratio", "", 5, 1, 15);
    RooRealVar frac("frac", "", 0.5, 0, 1);
    RooRealVar shift("shift", "", 0, -10, 10);
    RooHILLdini model("model", "", *Bd_M, a, b, csi, shift, sigma, ratio, frac);

    // Perform fit
    RooFitResult * r = model.fitTo(*data, RooFit::Save(), 
            RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    r->Print("v");

    // Plot
    RooPlot * plt = Bd_M->frame();
    data->plotOn(plt, RooFit::MarkerStyle(1), RooFit::DrawOption("E"));
    model.plotOn(plt);
    RooHist * pulls = plt->pullHist();
    TCanvas * canvas = new TCanvas("canvas", "", 500, 1000);
    canvas->Divide(1, 2);
    canvas->cd(1);
    plt->Draw();
    canvas->cd(2);
    RooPlot * plt2 = Bd_M->frame();
    plt2->addPlotable(pulls, "BEX0");
    plt2->Draw();
    canvas->SaveAs("../Plots/DKpipi_2.pdf");








}
