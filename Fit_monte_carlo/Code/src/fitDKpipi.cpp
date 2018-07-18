#include <fstream>
#include <iostream>

#include "TApplication.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TChain.h"
#include "TFile.h"
#include "TH1F.h"
#include "TLegend.h"
#include "TString.h"

#include "RooGlobalFunc.h"
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
#include "RooKeysPdf.h"

#include "PlotStyle.hpp"

// ==========================================
// Fit to B+ -> D0 K*0 pi+ shape with RooHILL
// ==========================================
int main(int argc, char * argv[]) {

    // Get histogram
    TFile * file = TFile::Open("/home/pullen/analysis/B02DKstar/Backgrounds/"
            "DKpipi/Results/histograms.root", "READ");
    TH1F * hist = (TH1F*)file->Get("Bd_M");

    // Setup
    double min = 5000;
    double max = 5180;
    RooRealVar * Bd_M = new RooRealVar("Bd_M", "", min, max);
    RooDataSet * data = (RooDataSet*)(new RooDataHist("data", "", *Bd_M, RooFit::Import(*hist)));

    // Make parameter file
    std::ofstream pfile("../Results/DKpipi.param");

    // Gaussian + HILL fit
    setPlotStyle();
    gROOT->ForceStyle();

    // Make RooHILLdini
    // RooRealVar a("a", "", 4700, 5000, 4400);
    RooRealVar a("a", "", 4950, 5050, 4800);
    RooRealVar b("b", "", 5139.69);
    // RooRealVar csi("csi", "", 0, -1, 1);
    RooRealVar csi("csi", "", 1);
    RooRealVar sigma("sigma", "", 5, 0, 100);
    RooRealVar ratio("ratio", "", 5, 1, 15);
    RooRealVar frac("frac", "", 0.5, 0, 1);
    RooRealVar shift("shift", "", 0);
    RooHILLdini hill("hill", "", *Bd_M, a, b, csi, shift, sigma, ratio, frac);

    // RooKeysPdf model("model", "model", *Bd_M, *data, RooKeysPdf::MirrorBoth);

    // Make Gaussian
    RooRealVar sigma_gauss1("sigma_gauss1", "", 50, 0, 200);
    RooRealVar sigma_gauss2("sigma_gauss2", "", 50, 0, 200);
    // RooFormulaVar mean_gauss("mean_gauss", "(@0 + @1)/2", RooArgList(a, b));
    RooRealVar mean_gauss1("mean_gauss1", "", 5000, 4700, 5025);
    RooRealVar mean_gauss2("mean_gauss2", "", 5050, 5025, 5100);
    RooGaussian gauss1("gauss1", "", *Bd_M, mean_gauss1, sigma_gauss1);
    RooGaussian gauss2("gauss2", "", *Bd_M, mean_gauss2, sigma_gauss2);
    RooRealVar f_gauss1("f_gauss1", "", 0.2, 0, 1);
    RooRealVar f_gauss2("f_gauss2", "", 0.4, 0, 1);
    RooAddPdf model("model", "", RooArgList(gauss1, gauss2, hill), 
            RooArgList(f_gauss1, f_gauss2));

    // Perform fit
    RooFitResult * r = model.fitTo(*data, RooFit::Save(),
            RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    r->Print("v");

    // Save results
    pfile << "a " << a.getVal() << " " << a.getError() << std::endl;
    pfile << "b " << b.getVal() << " " << b.getError() << std::endl;
    pfile << "csi " << csi.getVal() << " " << csi.getError() << std::endl;
    pfile << "sigma " << sigma.getVal() << " " << sigma.getError() << std::endl;
    pfile << "ratio " << ratio.getVal() << " " << ratio.getError() << std::endl;
    pfile << "frac " << frac.getVal() << " " << frac.getError() << std::endl;
    pfile << "mean_gauss1 " << mean_gauss1.getVal() << " " << mean_gauss1.getError() << std::endl;
    pfile << "sigma_gauss1 " << sigma_gauss1.getVal() << " " << sigma_gauss1.getError() << std::endl;
    pfile << "f_gauss1 " << f_gauss1.getVal() << " " << f_gauss1.getError() << std::endl;
    pfile << "mean_gauss2 " << mean_gauss2.getVal() << " " << mean_gauss2.getError() << std::endl;
    pfile << "sigma_gauss2 " << sigma_gauss2.getVal() << " " << sigma_gauss2.getError() << std::endl;
    pfile << "f_gauss2 " << f_gauss2.getVal() << " " << f_gauss2.getError() << std::endl;
    pfile.close();

    // Plot
    RooPlot * plt = Bd_M->frame();
    data->plotOn(plt, RooFit::MarkerStyle(1), RooFit::DrawOption("E"));
    model.plotOn(plt, RooFit::LineColor(kBlack));
    RooHist * pulls = plt->pullHist();
    model.plotOn(plt, RooFit::Components("gauss1"), RooFit::LineColor(kRed));
    model.plotOn(plt, RooFit::Components("gauss2"), RooFit::LineColor(kGreen));
    model.plotOn(plt, RooFit::Components("hill"), RooFit::LineColor(kBlue));
    TCanvas * canvas = new TCanvas("canvas", "", 900, 900);
    TPad * pad1 = new TPad("plot", "", 0.0, 1.0/3, 1.0, 1.0);
    pad1->cd();
    plt->Draw();
    TPad * pad2 = new TPad("pullplot", "", 0.0, 0.0, 1.0, 1.0/3);
    pad2->cd();
    RooPlot * plt2 = Bd_M->frame();
    plt2->addPlotable(pulls, "BEX0");
    plt2->Draw();
    canvas->cd();
    pad1->Draw();
    pad2->Draw();
    canvas->SaveAs("../Plots/DKpipi.pdf");

    // Save histogram
    TH1F * model_hist = (TH1F*)model.createHistogram("hist", *Bd_M,
            RooFit::Binning(10 * Bd_M->getBins()));
    TFile * hist_file = TFile::Open("../Histograms/DKpipi_RapidSim.root", "RECREATE");
    model_hist->Write();
    hist_file->Close();
}
