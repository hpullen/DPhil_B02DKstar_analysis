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
#include "RooSimultaneous.h"
#include "RooCategory.h"
#include "RooGaussian.h"
#include "RooDataSet.h"
#include "RooFitResult.h"
#include "RooFormulaVar.h"
#include "RooHist.h"
#include "RooPlotable.h"
#include "RooPlot.h"
#include "RooRealVar.h"
#include "RooCruijff.h"

#include "Plotter.hpp"

// Program for fitting to signal MC with a double crystal ball shape
int main(int argc, char * argv[]) {

    // Input args
    bool no_DTF = false;
    TString extra = "";
    if (argc == 2) {
        if (argv[1] == std::string("--noDTF")) {
            no_DTF = true;
            extra = "_noDTF";
        }
    }

    // Load two-body signal MC
    std::string path = "/data/lhcb/users/pullen/B02DKstar/MC/";
    TChain *  tree_two = new TChain("DecayTree");
    tree_two->Add((path + "twoBody/Kpi/2015_down/Kpi_selected.root").c_str());
    tree_two->Add((path + "twoBody/Kpi/2015_down/Kpi_selected.root").c_str());
    tree_two->Add((path + "twoBody/Kpi/2016_down/Kpi_selected.root").c_str());
    tree_two->Add((path + "twoBody/Kpi/2016_up/Kpi_selected.root").c_str());

    // Load four-body signal MC
    TChain *  tree_four = new TChain("DecayTree");
    tree_four->Add((path + "fourBody/Kpipipi/2016_down/Kpipipi_selected.root").c_str());
    tree_four->Add((path + "fourBody/Kpipipi/2016_up/Kpipipi_selected.root").c_str());

    // Set variables
    TString var_name = no_DTF ? "Bd_M" : "Bd_ConsD_MD";
    RooRealVar Bd_M(var_name, "", 5160, 5400, "MeV/c^{2}");
    RooRealVar bkgcat("Bd_BKGCAT", "", 0);

    // Set up bins
    int binWidth = 2;
    double nBins = (Bd_M.getMax() - Bd_M.getMin()) / binWidth;
    Bd_M.setBins(nBins);

    // Make args lists
    RooArgList args(Bd_M, bkgcat);

    // Make RooDataSets
    RooDataSet * data_two = new RooDataSet("data_two", "", tree_two, args);
    RooDataSet * data_four = new RooDataSet("data_four", "", tree_four, args);

    // Basic fit parameters
    RooRealVar * mean_Kpi = new RooRealVar("mean_Kpi", "", 5280, 5200, 5400);
    RooRealVar * mean_Kpipipi = new RooRealVar("mean_Kpipipi", "", 5280, 
            5200, 5400);
    RooRealVar * sigma_Kpi = new RooRealVar("sigma_Kpi", "", 15, 0, 50);
    RooRealVar * alpha_L = new RooRealVar("alpha_L", "", 1.5, 0, 3);
    RooRealVar * alpha_R = new RooRealVar("alpha_R", "", 0.1, 0, 3);

    // Set up ratios between widths
    RooRealVar * sigma_ratio = new RooRealVar("sigma_ratio", "", 1, 0, 10);
    RooRealVar * sigma_two_vs_four = new RooRealVar("sigma_two_vs_four", "", 1, 0, 10);
    RooFormulaVar * sigma_Kpipipi = new RooFormulaVar("sigma_Kpipipi", "@0 * @1", 
            RooArgList(*sigma_Kpi, *sigma_two_vs_four));
    RooFormulaVar * sigma2_Kpi = new RooFormulaVar("sigma2_Kpi", "@0 * @1", 
            RooArgList(*sigma_Kpi, *sigma_ratio));
    RooFormulaVar * sigma2_Kpipipi = new RooFormulaVar("sigma2_Kpipipi", "@0 * @1", 
            RooArgList(*sigma_Kpipipi, *sigma_ratio));

    // PDFs
    RooCruijff * fit_Kpi = new RooCruijff("Kpi", "", Bd_M, *mean_Kpi, *sigma_Kpi,
            *sigma2_Kpi, *alpha_L, *alpha_R);
    RooCruijff * fit_Kpipipi = new RooCruijff("Kpipipi", "", Bd_M, *mean_Kpipipi, 
            *sigma_Kpipipi, *sigma2_Kpipipi, *alpha_L, *alpha_R);

    // Set up simultaneous fit
    RooCategory category("category" , "");
    category.defineType("two");
    category.defineType("four");
    std::map<std::string, RooDataSet *> dataMap = {{"two", data_two},
        {"four", data_four}};
    RooDataSet combData("combData", "", Bd_M, RooFit::Index(category),
            RooFit::Import(dataMap));
    RooSimultaneous * simPdf = new RooSimultaneous("simPdf", "", category);
    simPdf->addPdf(*fit_Kpi, "two");
    simPdf->addPdf(*fit_Kpipipi, "four");

    // Fit to the dataset
    RooFitResult * r = simPdf->fitTo(combData, RooFit::Save(), RooFit::NumCPU(8, 2),
            RooFit::Optimize(false), RooFit::Offset(true), 
            RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    r->Print("v");

    // Save output to a file
    std::ofstream params("../Results/combined_twoBody_fourBody" + extra + ".param");
    params << "mean_Kpi " << mean_Kpi->getVal() << " " << mean_Kpi->getError() << std::endl;
    params << "mean_Kpipipi " << mean_Kpipipi->getVal() << " " << mean_Kpipipi->getError() << std::endl;
    params << "sigma_Kpi " << sigma_Kpi->getVal() << " " << sigma_Kpi->getError() << std::endl;
    params << "sigma_ratio " << sigma_ratio->getVal() << " " << sigma_ratio->getError() << std::endl;
    params << "sigma_two_vs_four " << sigma_two_vs_four->getVal() << " " << sigma_two_vs_four->getError() << std::endl;
    params.close();

    // Convert PDFs to TH1s
    TH1F * h_data_two = (TH1F*)data_two->createHistogram("hData_two", Bd_M);
    TH1F * h_fit_two = (TH1F*)fit_Kpi->createHistogram("hFit_two", Bd_M, 
            RooFit::Binning(nBins * 10));
    TH1F * h_data_four = (TH1F*)data_four->createHistogram("hData_four", Bd_M);
    TH1F * h_fit_four = (TH1F*)fit_Kpipipi->createHistogram("hFit_four", Bd_M, 
            RooFit::Binning(nBins * 10));

    // Scale histograms
    h_fit_two->Scale(h_data_two->Integral() * 10);
    h_fit_four->Scale(h_data_four->Integral() * 10);

    // Plot the fit and display it in TApplication
    RooPlot * frame = Bd_M.frame(RooFit::Title(""));
    data_two->plotOn(frame);
    fit_Kpi->plotOn(frame);
    RooHist * pulls_two = frame->pullHist();
    RooPlot * frame_four = Bd_M.frame(RooFit::Title(""));
    data_four->plotOn(frame_four);
    fit_Kpipipi->plotOn(frame_four);
    RooHist * pulls_four = frame_four->pullHist();

    // Save two-body histograms
    TFile * outfile_two = TFile::Open("../Histograms/shared_cruijff" + extra + "_Kpi.root", "RECREATE");
    outfile_two->cd();
    h_data_two->Write("data");
    h_fit_two->Write("fit");
    pulls_two->Write("pulls");
    outfile_two->Close();

    // Save four-body histograms
    TFile * outfile_four = TFile::Open("../Histograms/shared_cruijff" + extra + "_Kpipipi.root", "RECREATE");
    outfile_four->cd();
    h_data_four->Write("data");
    h_fit_four->Write("fit");
    pulls_four->Write("pulls");
    outfile_four->Close();

    // Plot the results nicely
    Plotter * plotter = new Plotter("Kpi", "shared_cruijff" + std::string(extra));
    plotter->plotFit();
    delete plotter;
    Plotter * plotter_four = new Plotter("Kpipipi", "shared_cruijff" + std::string(extra));
    plotter->plotFit();
    delete plotter_four;

    // Display plot in TApplication
    //app->Run();
    
    return 0;
}
