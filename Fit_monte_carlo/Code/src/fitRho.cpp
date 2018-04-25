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

    // Get PID cut option
    std::string cuts = "none";
    if (argc > 1) cuts = argv[1];
    std::string name;
    if (cuts == "none") {
        name = "noPIDcut";
    } else if (cuts == "KstarK") {
        name = "KstarK_PIDcut";
    }   else if (cuts == "all") {
        name = "all_PIDcut";
    } else {
        std::cout << "Usage: ./FitRho <none/KstarK/all>" << std::endl;
        return -1;
    }

    // Load rho MC
    std::string path = "/data/lhcb/users/pullen/B02DKstar/MC/backgrounds/rho/";
    TChain *  tree = new TChain("DecayTree");
    tree->Add((path + "2016_down/Kpi_withVars_withCuts_resampled.root").c_str());
    tree->Add((path + "2016_up/Kpi_withVars_withCuts_resampled.root").c_str());

    // Set variables
    // To do: add double mis-ID cut to ADS modes
    RooRealVar Bd_M("Bd_ConsD_MD", "", 5200, 5500, "MeV/c^{2}");
    RooRealVar D0_M("D0_M", "", 1864.83 - 25, 1864.64 + 25);
    RooRealVar D0_FDS("D0_FDS", "", 2, 100000000);
    RooRealVar BDTG("BDTG_Kpi_run2", "", 0.5, 1);
    RooRealVar Bd_BKGCAT("Bd_BKGCAT", "", 30);
    RooRealVar KstarK_PIDK("KstarK_PIDK_corr", "", 3, 100000000);
    RooRealVar KstarPi_PIDK("KstarPi_PIDK_corr", "", -100000000, -1);
    RooRealVar D0K_PIDK("D0K_PIDK_corr", "", 1, 100000000);
    RooRealVar D0Pi_PIDK("D0Pi_PIDK_corr", "", -100000000, -1);

    // Set up bins
    int binWidth = 5;
    double nBins = (Bd_M.getMax() - Bd_M.getMin()) / binWidth;
    Bd_M.setBins(nBins);

    // Make args and dataset
    RooArgList args;
    args.add(Bd_M);
    args.add(D0_M);
    args.add(D0_FDS);
    args.add(BDTG);
    args.add(Bd_BKGCAT);
    if (cuts == "KstarK" || cuts == "all") args.add(KstarK_PIDK);
    if (cuts == "all") {
        args.add(KstarPi_PIDK);
        args.add(D0K_PIDK);
        args.add(D0Pi_PIDK);
    }
    RooDataSet * data = new RooDataSet("data", "", tree, args);

    // Fit parameters
    RooRealVar * mean = new RooRealVar("mean", "", 5320, 5200, 5500);
    RooRealVar * sigma_L = new RooRealVar("sigma_L", "", 15, 0, 50);
    RooRealVar * sigma_ratio = new RooRealVar("sigma_ratio", "", 1, 0, 20);
    RooFormulaVar * sigma_R = new RooFormulaVar("sigma_R", "@0 * @1", RooArgList(*sigma_L, *sigma_ratio));
    RooRealVar * alpha_L = new RooRealVar("alpha_L", "", 3, 0.5, 10);
    RooRealVar * alpha_R = new RooRealVar("alpha_R", "", -2, -10, -0.001);
    RooRealVar * n_L = new RooRealVar("n_L", "", 1, 0, 10);
    RooRealVar * n_R = new RooRealVar("n_R", "", 1, 0, 10);
    RooRealVar * frac = new RooRealVar("frac", "", 0.5, 0, 1);

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
    std::ofstream params("../Results/rho_" + name + ".param");
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
    TFile * outfile = TFile::Open(("../Histograms/rho_" + name + "_Kpi.root").c_str(), "RECREATE");
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
    Plotter * plotter = new Plotter("Kpi", "rho_" + name);
    plotter->plotFit("CB_L", "CB_R");

    // Display plot in TApplication
    //app->Run();
    
    return 0;
}
