#include <iostream>
#include <fstream>

#include "TApplication.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TChain.h"
#include "TH1F.h"


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
            tree_B0->Add(path + "backgrounds/Bs/" + year + mag + "/Kpi_selected.root");
        }
    }

    // Set variables
    double mass_diff = 87.26;
    RooRealVar Bd_M("Bd_ConsD_MD", "", 5160, 5490, "MeV/c^{2}");

    // Set up bins
    int binWidth = 2;
    double nBins = (Bd_M.getMax() - Bd_M.getMin()) / binWidth;
    Bd_M.setBins(nBins);

    // Make category
    RooCategory cat;
    cat.defineType("B0");
    cat.defineType("Bs");

    // Make args and dataset
    RooArgList args;
    args.add(Bd_M);
    RooDataSet * data_B0 = new RooDataSet("data_B0", "", tree_B0, args);
    RooDataSet * data_Bs = new RooDataSet("data_Bs", "", tree_Bs, args);
    RooDataSet * combData = new RooDataSet("combData", "", Bd_M, 
            RooFit::Index(cat), RooFit::Import("B0", *data_B0),
            RooFit::Import("Bs", *data_Bs));

    // // Fit parameters
    // RooRealVar * mean = new RooRealVar("mean", "", 5280 + mass_diff, 5200 + mass_diff, 5400 + mass_diff);
    // RooRealVar * sigma_L = new RooRealVar("sigma_L", "", 12, 0, 50);
    // // RooRealVar * sigma_ratio = new RooRealVar("sigma_ratio", "", 1, 0, 10);
    // // RooFormulaVar * sigma_R = new RooFormulaVar("sigma_R", "@0 * @1", RooArgList(*sigma_L, *sigma_ratio));
    // RooRealVar * alpha_L = new RooRealVar("alpha_L", "", 1.5, 0, 3);
    // RooRealVar * alpha_R = new RooRealVar("alpha_R", "", -1.5, -3, -0.001);
    // RooRealVar * n_L = new RooRealVar("n_L", "", 6, 0, 10);
    // RooRealVar * n_R = new RooRealVar("n_R", "", 6, 0, 10);
    // RooRealVar * frac = new RooRealVar("frac", "", 0.5, 0, 1);

    // // PDFs
    // RooCBShape * signal_L = new RooCBShape("signal_L", "", Bd_M, *mean, *sigma_L,
            // *alpha_L, *n_L);
    // RooCBShape * signal_R = new RooCBShape("signal_R", "", Bd_M, *mean, *sigma_L,
            // *alpha_R, *n_R);
    // RooAddPdf * signal = new RooAddPdf("signal", "", RooArgList(*signal_L,
                // *signal_R), RooArgList(*frac));

    // // Fit to the dataset
    // std::cout << "Dataset entries: " << data->sumEntries() << std::endl;
    // RooFitResult * r = signal->fitTo(*data, RooFit::Save(), RooFit::NumCPU(8, 2),
            // RooFit::Optimize(false), RooFit::Offset(false),
            // RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    // r->Print("v");

    // // Save output to a file
    // std::string extra = no_dtf ? "_noDTF" : "";
    // std::ofstream params("../Results/signal_" + mode + extra + ".param");
    // params << "alpha_L " << alpha_L->getVal() << " " << alpha_L->getError() << std::endl;
    // params << "alpha_R " << alpha_R->getVal() << " " << alpha_R->getError() << std::endl;
    // params << "frac " << frac->getVal() << " " << frac->getError() << std::endl;
    // params << "mean " << mean->getVal() << " " << mean->getError() << std::endl;
    // params << "n_L " << n_L->getVal() << " " << n_L->getError() << std::endl;
    // params << "n_R " << n_R->getVal() << " " << n_R->getError() << std::endl;
    // params << "sigma_L " << sigma_L->getVal() << " " << sigma_L->getError() << std::endl;
    // // params << "sigma_ratio " << sigma_ratio->getVal() << " " << sigma_ratio->getError() << std::endl;
    // params.close();

    // // Convert PDFs to TH1s
    // TFile * outfile = TFile::Open(("../Histograms/signal" + extra + "_" + mode + ".root").c_str(),
            // "RECREATE");
    // TH1F * h_data = (TH1F*)data->createHistogram("data", Bd_M);
    // TH1F * h_fit = (TH1F*)signal->createHistogram("fit", Bd_M,
            // RooFit::Binning(nBins * 10));
    // TH1F * h_CB_L = (TH1F*)signal_L->createHistogram("CB_L", Bd_M,
            // RooFit::Binning(nBins * 10));
    // TH1F * h_CB_R = (TH1F*)signal_R->createHistogram("CB_R", Bd_M,
            // RooFit::Binning(nBins * 10));

    // // Scale histograms
    // h_fit->Scale(h_data->Integral() * 10);
    // h_CB_L->Scale(h_data->Integral() * 10 * frac->getVal());
    // h_CB_R->Scale(h_data->Integral() * 10 * (1 - frac->getVal()));

    // // Plot the fit and display it in TApplication
    // RooPlot * frame = Bd_M.frame(RooFit::Title(""));
    // data->plotOn(frame);
    // signal->plotOn(frame);
    // RooHist * pulls = frame->pullHist();

    // // Save histograms and pulls to file
    // outfile->cd();
    // h_data->Write("data");
    // h_fit->Write("fit");
    // h_CB_L->Write("CB_L");
    // h_CB_R->Write("CB_R");
    // pulls->Write("pulls");
    // outfile->Close();

    // // Plot the results nicely
    // Plotter * plotter = new Plotter(mode, "signal" + extra);
    // plotter->plotFit("CB_L", "CB_R");

    // Display plot in TApplication
    //app->Run();
    
    return 0;
}
