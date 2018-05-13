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

// Program for fitting to rho MC with a double crystal ball shape
int main(int argc, char * argv[]) {

    // Make a TApplication to display output
    //TApplication * app = new TApplication("app", &argc, argv);

    // Load rho MC
    std::string path = "/data/lhcb/users/pullen/B02DKstar/MC/backgrounds/rho/";
    TChain *  tree = new TChain("DecayTree");
    tree->Add((path + "2016_down/Kpi_selected_resampled.root").c_str());
    tree->Add((path + "2016_up/Kpi_selected_resampled.root").c_str());

    // Set variables
    // To do: add double mis-ID cut to ADS modes
    RooRealVar Bd_M("Bd_ConsD_MD", "", 5200, 5500, "MeV/c^{2}");
    RooRealVar KstarK_PIDK("KstarK_PIDK", "", 5, 1000000);

    // Set up bins
    int binWidth = 5;
    double nBins = (Bd_M.getMax() - Bd_M.getMin()) / binWidth;
    Bd_M.setBins(nBins);

    // Make args and dataset
    RooArgList args;
    args.add(Bd_M);
    args.add(KstarK_PIDK);
    RooDataSet * data = new RooDataSet("data", "", tree, args);

    // Fit parameters
    RooRealVar * mean = new RooRealVar("mean", "", 5320, 5200, 5500);
    RooRealVar * sigma_L = new RooRealVar("sigma_L", "", 25, 0, 50);
    RooRealVar * sigma_ratio = new RooRealVar("sigma_ratio", "", 0.02, 0, 1);
    RooFormulaVar * sigma_R = new RooFormulaVar("sigma_R", "@0 * @1", RooArgList(*sigma_L, *sigma_ratio));
    RooRealVar * alpha_L = new RooRealVar("alpha_L", "", 1.5, 0.5, 10);
    RooRealVar * alpha_R = new RooRealVar("alpha_R", "", -0.3, -5, -0.001);
    RooRealVar * n_L = new RooRealVar("n_L", "", 1, 0, 10);
    RooRealVar * n_R = new RooRealVar("n_R", "", 9, 0, 10);
    RooRealVar * frac = new RooRealVar("frac", "", 0.2, 0, 1);

    // PDFs
    RooCBShape * signal_L = new RooCBShape("signal_L", "", Bd_M, *mean, *sigma_L,
            *alpha_L, *n_L);
    RooCBShape * signal_R = new RooCBShape("signal_R", "", Bd_M, *mean, *sigma_R,
            *alpha_R, *n_R);
    RooAddPdf * signal = new RooAddPdf("signal", "", RooArgList(*signal_L, 
                *signal_R), RooArgList(*frac));

    // Fit to the dataset
    std::cout << "Dataset entries: " << data->sumEntries() << std::endl;
    RooFitResult * r = signal->fitTo(*data, RooFit::Save(), RooFit::NumCPU(8, 2),
            RooFit::Optimize(false), RooFit::Offset(true), 
            RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    r->Print("v");

    // Save output to a file
    std::ofstream params("../Results/rho.param");
    params << "alpha_L " << alpha_L->getVal() << " " << alpha_L->getError() << std::endl;
    params << "alpha_R " << alpha_R->getVal() << " " << alpha_R->getError() << std::endl;
    params << "frac " << frac->getVal() << " " << frac->getError() << std::endl;
    params << "mean " << mean->getVal() << " " << mean->getError() << std::endl;
    params << "n_L " << n_L->getVal() << " " << n_L->getError() << std::endl;
    params << "n_R " << n_R->getVal() << " " << n_R->getError() << std::endl;
    params << "sigma_L " << sigma_L->getVal() << " " << sigma_L->getError() << std::endl;
    params << "sigma_ratio " << sigma_ratio->getVal() << " " << sigma_ratio->getError() << std::endl;
    params.close();

    // Convert PDFs to TH1s
    TFile * outfile = TFile::Open("../Histograms/rho_Kpi.root", "RECREATE");
    TH1F * h_data = (TH1F*)data->createHistogram("data", Bd_M);
    TH1F * h_fit = (TH1F*)signal->createHistogram("fit", Bd_M, 
            RooFit::Binning(nBins * 10));
    TH1F * h_CB_L = (TH1F*)signal_L->createHistogram("CB_L", Bd_M, 
            RooFit::Binning(nBins * 10));
    TH1F * h_CB_R = (TH1F*)signal_R->createHistogram("CB_R", Bd_M, 
            RooFit::Binning(nBins * 10));

    // Scale histograms
    h_fit->Scale(h_data->Integral() * 10);
    h_CB_L->Scale(h_data->Integral() * 10 * frac->getVal());
    h_CB_R->Scale(h_data->Integral() * 10 * (1 - frac->getVal()));

    // Plot the fit and display it in TApplication
    RooPlot * frame = Bd_M.frame(RooFit::Title(""));
    data->plotOn(frame);
    signal->plotOn(frame);
    RooHist * pulls = frame->pullHist();

    // Save histograms and pulls to file
    outfile->cd();
    h_data->Write("data");
    h_fit->Write("fit");
    h_CB_L->Write("CB_L");
    h_CB_R->Write("CB_R");
    pulls->Write("pulls");
    outfile->Close();

    // Plot the results nicely
    Plotter * plotter = new Plotter("Kpi", "rho");
    plotter->plotFit("CB_L", "CB_R");

    // Display plot in TApplication
    //app->Run();
    
    return 0;
}
