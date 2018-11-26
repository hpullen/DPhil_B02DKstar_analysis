#include <iostream>
#include <fstream>

#include "TApplication.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TChain.h"
#include "TH1F.h"

#include "RooCategory.h"
#include "RooGaussian.h"
#include "RooSimultaneous.h"
#include "RooGaussian.h"
#include "RooGlobalFunc.h"
#include "RooAddPdf.h"
#include "RooArgList.h"
#include "RooArgSet.h"
#include "RooCBShape.h"
#include "RooDataSet.h"
#include "RooFitResult.h"
#include "RooFormulaVar.h"
#include "RooHist.h"
#include "RooPlotable.h"
#include "RooPlot.h"
#include "RooRealVar.h"

#include "Plotter.hpp"

// Program for fitting to B0 and Bs MC simultaneously
int main(int argc, char * argv[]) {

    // Load MC for B0 and Bs
    std::string path = "/data/lhcb/users/pullen/B02DKstar/MC/";
    TChain *  tree_B0 = new TChain("DecayTree");
    TChain *  tree_Bs = new TChain("DecayTree");
    for (TString year : {"2015", "2016"}) {
        for (TString mag : {"_up", "_down"}) {
            tree_B0->Add(path + "twoBody/Kpi/" + year + mag + "/Kpi_selected.root");
            tree_Bs->Add(path + "backgrounds/Bs/" + year + mag + "/Kpi_selected.root");
        }
    }

    // Set variables
    RooRealVar Bd_M("Bd_ConsD_MD", "", 5160, 5490, "MeV/c^{2}");

    // Set up bins
    int binWidth = 2;
    double nBins = (Bd_M.getMax() - Bd_M.getMin()) / binWidth;
    Bd_M.setBins(nBins);

    // Make category
    RooCategory * cat = new RooCategory();
    cat->defineType("B0");
    cat->defineType("Bs");

    // Make args and dataset
    RooArgList args;
    args.add(Bd_M);
    RooDataSet * data_B0 = new RooDataSet("data_B0", "", tree_B0, args);
    RooDataSet * data_Bs = new RooDataSet("data_Bs", "", tree_Bs, args);
    RooDataSet * data = new RooDataSet("data", "", Bd_M, RooFit::Index(*cat), 
            RooFit::Import("B0", *data_B0),
            RooFit::Import("Bs", *data_Bs));

    // Fit parameters: B0
    RooRealVar mean("mean", "", 5280, 5200, 5400);
    RooRealVar sigma("sigma", "", 12, 0, 50);
    RooGaussian * pdf_B0 = new RooGaussian("pdf_B0", "", Bd_M, mean, sigma);

    // Fit parameters: Bs
    RooRealVar delta_M("delta_M", "", 87.26);
    // RooFormulaVar mean_Bs("mean_Bs", "", "@0 + @1", RooArgList(mean, delta_M));
    RooRealVar mean_Bs("mean_Bs", "", 5370, 5290, 5490);
    RooRealVar sigma_Bs("sigma_Bs", "", 12, 0, 50);
    RooRealVar alpha_L("alpha_L", "", 1.5, 0, 3);
    RooRealVar alpha_R("alpha_R", "", -1.5, -3, -0.001);
    RooRealVar n_L("n_L", "", 6, 0, 10);
    RooRealVar n_R("n_R", "", 6, 0, 10);
    RooRealVar frac("frac", "", 0.5, 0, 1);
    RooCBShape * pdf_Bs_L = new RooCBShape("pdf_Bs_L", "", Bd_M, mean_Bs, sigma_Bs,
        alpha_L, n_L);
    RooCBShape * pdf_Bs_R = new RooCBShape("pdf_Bs_R", "", Bd_M, mean_Bs, sigma_Bs,
        alpha_R, n_R);
    RooAddPdf * pdf_Bs = new RooAddPdf("pdf_Bs", "", RooArgList(*pdf_Bs_L, *pdf_Bs_R),
        RooArgList(frac));

    // Make simultaneous PDF
    RooSimultaneous * pdf = new RooSimultaneous("pdf", "", *cat);
    pdf->addPdf(*pdf_B0, "B0");
    pdf->addPdf(*pdf_Bs, "Bs");

    // Fit to the dataset
    RooFitResult * r = pdf->fitTo(*data, RooFit::Save(), RooFit::NumCPU(8, 2),
            RooFit::Optimize(false), RooFit::Offset(false), 
            RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    r->Print("v");

    // Save output to a file
    std::ofstream params("../Results/signal_both.param");
    params << "mean " << mean.getVal() << " " << mean.getError() << std::endl;
    params << "delta_M " << delta_M.getVal() << " " << "0.24" << std::endl;
    params << "sigma " << sigma.getVal() << " " << sigma.getError() << std::endl;
    params << "sigma_Bs " << sigma_Bs.getVal() << " " << sigma_Bs.getError() << std::endl;
    params << "alpha_L " << alpha_L.getVal() << " " << alpha_L.getError() << std::endl;
    params << "alpha_R " << alpha_R.getVal() << " " << alpha_R.getError() << std::endl;
    params << "frac " << frac.getVal() << " " << frac.getError() << std::endl;
    params << "n_L " << n_L.getVal() << " " << n_L.getError() << std::endl;
    params << "n_R " << n_R.getVal() << " " << n_R.getError() << std::endl;
    params.close();

    // Convert PDFs to TH1s
    TFile * outfile_B0 = TFile::Open("../Histograms/signal_B0.root", "RECREATE");
    TFile * outfile_Bs = TFile::Open("../Histograms/signal_Bs.root", "RECREATE");
    TH1F * h_data_B0 = (TH1F*)data_B0->createHistogram("data_B0", Bd_M);
    TH1F * h_data_Bs = (TH1F*)data_Bs->createHistogram("data_Bs", Bd_M);
    TH1F * h_fit_B0 = (TH1F*)pdf_B0->createHistogram("fit_B0", Bd_M, RooFit::Binning(nBins * 10));
    TH1F * h_fit_Bs = (TH1F*)pdf_Bs->createHistogram("fit_Bs", Bd_M, RooFit::Binning(nBins * 10));
    TH1F * h_CB_L = (TH1F*)pdf_Bs_L->createHistogram("CB_L", Bd_M, RooFit::Binning(nBins * 10));
    TH1F * h_CB_R = (TH1F*)pdf_Bs_R->createHistogram("CB_R", Bd_M, RooFit::Binning(nBins * 10));

    // Scale histograms
    h_fit_B0->Scale(h_data_B0->Integral() * 10);
    h_fit_Bs->Scale(h_data_Bs->Integral() * 10);
    h_CB_L->Scale(h_data_Bs->Integral() * 10 * frac.getVal());
    h_CB_R->Scale(h_data_Bs->Integral() * 10 * (1 - frac.getVal()));

    // Make pulls for each
    RooPlot * plot_B0 = Bd_M.frame();
    data_B0->plotOn(plot_B0);
    pdf_B0->plotOn(plot_B0);
    RooHist * pulls_B0 = plot_B0->pullHist();
    RooPlot * plot_Bs = Bd_M.frame();
    data_Bs->plotOn(plot_Bs);
    pdf_Bs->plotOn(plot_Bs);
    RooHist * pulls_Bs = plot_Bs->pullHist();

    // Save histograms and pulls to file
    outfile_B0->cd();
    h_data_B0->Write("data");
    h_fit_B0->Write("fit");
    pulls_B0->Write("pulls");
    outfile_B0->Close();

    // Save to Bs file
    outfile_Bs->cd();
    h_data_Bs->Write("data");
    h_fit_Bs->Write("fit");
    h_CB_L->Write("CB_L");
    h_CB_R->Write("CB_R");
    pulls_Bs->Write("pulls");
    outfile_Bs->Close();

    // Plot the results nicely
    Plotter * plotter_B0 = new Plotter("B0", "signal");
    plotter_B0->plotFit();
    Plotter * plotter_Bs = new Plotter("Bs", "signal");
    plotter_Bs->plotFit("CB_L", "CB_R");

    return 0;
}
