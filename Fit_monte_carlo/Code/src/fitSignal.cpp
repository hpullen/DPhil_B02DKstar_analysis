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

// Program for fitting to signal MC with a double crystal ball shape
int main(int argc, char * argv[]) {

    // Make a TApplication to display output
    //TApplication * app = new TApplication("app", &argc, argv);

    // Get D0 decay mode
    if (argc != 2) {
        std::cout << "Usage: ./FitSignal <D0-mode>" << std::endl;
        return -1;
    }
    std::string mode = argv[1];
    bool is_twoBody = true;
    bool is_Bs = false;
    bool is_run1 = false;
    if (mode == "Kpipipi" || mode == "pipipipi") is_twoBody = false;
    if (mode == "Bs") is_Bs = true;
    if (mode == "run1") is_run1 = true;

    // Load signal MC
    std::string path = "/data/lhcb/users/pullen/B02DKstar/MC/";
    TChain *  tree = new TChain("DecayTree");
    if (is_Bs) {
        tree->Add((path + "backgrounds/Bs/2015_down/Kpi_selected.root").c_str());
        tree->Add((path + "backgrounds/Bs/2015_up/Kpi_selected.root").c_str());
        tree->Add((path + "backgrounds/Bs/2016_down/Kpi_selected.root").c_str());
        tree->Add((path + "backgrounds/Bs/2016_up/Kpi_selected.root").c_str());
    } else if (is_run1) {
        tree->Add((path + "twoBody/Kpi/2011_down/Kpi_selected.root").c_str());
        tree->Add((path + "twoBody/Kpi/2011_up/Kpi_selected.root").c_str());
        tree->Add((path + "twoBody/Kpi/2012_down/Kpi_selected.root").c_str());
        tree->Add((path + "twoBody/Kpi/2012_up/Kpi_selected.root").c_str());
    } else {
        (is_twoBody) ? path += "twoBody/" : path += "fourBody/";
        path += mode + "/";
        tree->Add((path + "2016_down/" + mode + "_selected.root").c_str());
        tree->Add((path + "2016_up/" + mode + "_selected.root").c_str());
        if (is_twoBody) {
            tree->Add((path + "2015_down/" + mode + "_selected.root").c_str());
            tree->Add((path + "2015_up/" + mode + "_selected.root").c_str());
        }
    }

    // Set variables
    // To do: add double mis-ID cut to ADS modes
    double mass_diff = (is_Bs) ? 90 : 0;
    RooRealVar Bd_M("Bd_ConsD_MD", "", 5160 + mass_diff, 5400 + mass_diff, "MeV/c^{2}");

    // Set up bins
    int binWidth = 2;
    double nBins = (Bd_M.getMax() - Bd_M.getMin()) / binWidth;
    Bd_M.setBins(nBins);

    // Make args and dataset
    RooArgList args;
    args.add(Bd_M);
    RooDataSet * data = new RooDataSet("data", "", tree, args);
    std::cout << "Fitting to " << data->sumEntries() << " entries" << std::endl;

    // Fit parameters
    RooRealVar * mean = new RooRealVar("mean", "", 5280 + mass_diff, 5200 + mass_diff, 5400 + mass_diff);
    RooRealVar * sigma_L = new RooRealVar("sigma_L", "", 12, 0, 50);
    // RooRealVar * sigma_ratio = new RooRealVar("sigma_ratio", "", 1, 0, 10);
    // RooFormulaVar * sigma_R = new RooFormulaVar("sigma_R", "@0 * @1", RooArgList(*sigma_L, *sigma_ratio));
    RooRealVar * alpha_L = new RooRealVar("alpha_L", "", 1.5, 0, 3);
    RooRealVar * alpha_R = new RooRealVar("alpha_R", "", -1.5, -3, -0.001);
    RooRealVar * n_L = new RooRealVar("n_L", "", 6, 0, 10);
    RooRealVar * n_R = new RooRealVar("n_R", "", 6, 0, 10);
    RooRealVar * frac = new RooRealVar("frac", "", 0.5, 0, 1);

    // PDFs
    RooCBShape * signal_L = new RooCBShape("signal_L", "", Bd_M, *mean, *sigma_L,
            *alpha_L, *n_L);
    RooCBShape * signal_R = new RooCBShape("signal_R", "", Bd_M, *mean, *sigma_L,
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
    std::ofstream params("../Results/signal_" + mode + ".param");
    params << "alpha_L " << alpha_L->getVal() << " " << alpha_L->getError() << std::endl;
    params << "alpha_R " << alpha_R->getVal() << " " << alpha_R->getError() << std::endl;
    params << "frac " << frac->getVal() << " " << frac->getError() << std::endl;
    params << "mean " << mean->getVal() << " " << mean->getError() << std::endl;
    params << "n_L " << n_L->getVal() << " " << n_L->getError() << std::endl;
    params << "n_R " << n_R->getVal() << " " << n_R->getError() << std::endl;
    params << "sigma_L " << sigma_L->getVal() << " " << sigma_L->getError() << std::endl;
    // params << "sigma_ratio " << sigma_ratio->getVal() << " " << sigma_ratio->getError() << std::endl;
    params.close();

    // Convert PDFs to TH1s
    TFile * outfile = TFile::Open(("../Histograms/signal_" + mode + ".root").c_str(), 
            "RECREATE");
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
    Plotter * plotter = new Plotter(mode, "signal");
    plotter->plotFit("CB_L", "CB_R");

    // Display plot in TApplication
    //app->Run();
    
    return 0;
}
