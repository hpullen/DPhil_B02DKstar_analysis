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

#include "Plotter.hpp"

// Program for fitting to signal MC with a double crystal ball shape
int main(int argc, char * argv[]) {

    // Make a TApplication to display output
    //TApplication * app = new TApplication("app", &argc, argv);

    // Load two-body signal MC
    std::string path = "/data/lhcb/users/pullen/B02DKstar/MC/";
    TChain *  tree_two = new TChain("DecayTree");
    tree_two->Add((path + "twoBody/Kpi/2015_down/Kpi_withVars_withCuts.root").c_str());
    tree_two->Add((path + "twoBody/Kpi/2015_down/Kpi_withVars_withCuts.root").c_str());
    tree_two->Add((path + "twoBody/Kpi/2016_down/Kpi_withVars_withCuts.root").c_str());
    tree_two->Add((path + "twoBody/Kpi/2016_up/Kpi_withVars_withCuts.root").c_str());

    // Load four-body signal MC
    TChain *  tree_four = new TChain("DecayTree");
    tree_four->Add((path + "fourBody/Kpipipi/2016_down/Kpipipi_withVars_withCuts.root").c_str());
    tree_four->Add((path + "fourBody/Kpipipi/2016_up/Kpipipi_withVars_withCuts.root").c_str());

    // Set variables
    RooRealVar Bd_M("Bd_ConsD_MD", "", 5160, 5400, "MeV/c^{2}");
    RooRealVar D0_M("D0_M", "", 1864.83 - 25, 1864.64 + 25);
    RooRealVar D0_FDS("D0_FDS", "", 2, 100000000);
    RooRealVar BDTG_Kpi("BDTG_Kpi_run2", "", 0.5, 1);
    RooRealVar BDTG_Kpipipi("BDTG_Kpipipi_run2", "", 0.5, 1);
    RooRealVar Bd_BKGCAT("Bd_BKGCAT", "", 0);

    // Set up bins
    int binWidth = 2;
    double nBins = (Bd_M.getMax() - Bd_M.getMin()) / binWidth;
    Bd_M.setBins(nBins);

    // Make args lists
    RooArgList args_Kpi;
    args_Kpi.add(Bd_M);
    args_Kpi.add(D0_M);
    args_Kpi.add(D0_FDS);
    args_Kpi.add(Bd_BKGCAT);
    RooArgList args_Kpipipi = args_Kpi;
    args_Kpi.add(BDTG_Kpi);
    args_Kpipipi.add(BDTG_Kpipipi);

    // Make RooDataSets
    RooDataSet * data_two = new RooDataSet("data_two", "", tree_two, args_Kpi);
    RooDataSet * data_four = new RooDataSet("data_four", "", tree_four, args_Kpipipi);

    // Basic fit parameters
    RooRealVar * mean_Kpi = new RooRealVar("mean_Kpi", "", 5280, 5200, 5400);
    RooRealVar * mean_Kpipipi = new RooRealVar("mean_Kpipipi", "", 5280, 5200, 5400);
    RooRealVar * sigma_Kpi = new RooRealVar("sigma_Kpi", "", 15, 0, 50);
    RooRealVar * frac = new RooRealVar("frac", "", 0.5, 0, 1);

    // Set up ratios between widths
    RooRealVar * sigma_ratio = new RooRealVar("sigma_ratio", "", 1, 0, 10);
    RooRealVar * sigma_two_vs_four = new RooRealVar("sigma_two_vs_four", "", 1, 0, 10);
    RooFormulaVar * sigma_Kpipipi = new RooFormulaVar("sigma_Kpipipi", "@0 * @1", 
            RooArgList(*sigma_Kpi, *sigma_two_vs_four));
    RooFormulaVar * sigma2_Kpi = new RooFormulaVar("sigma2_Kpi", "@0 * @1", 
            RooArgList(*sigma_Kpi, *sigma_ratio));
    RooFormulaVar * sigma2_Kpipipi = new RooFormulaVar("sigma2_Kpipipi", "@0 * @1", 
            RooArgList(*sigma_Kpipipi, *sigma_ratio));

    // PDFs: two-body
    RooGaussian * gauss1_Kpi = new RooGaussian("gauss1_Kpi", "", Bd_M, *mean_Kpi, *sigma_Kpi);
    RooGaussian * gauss2_Kpi = new RooGaussian("gauss2_Kpi", "", Bd_M, *mean_Kpi, *sigma2_Kpi);
    RooAddPdf * fit_Kpi = new RooAddPdf("fit_Kpi", "", RooArgList(*gauss1_Kpi, *gauss2_Kpi),
            RooArgList(*frac));

    // PDFs: four-body
    RooGaussian * gauss1_Kpipipi = new RooGaussian("gauss1_Kpipipi", "", Bd_M, *mean_Kpipipi, *sigma_Kpipipi);
    RooGaussian * gauss2_Kpipipi = new RooGaussian("gauss2_Kpipipi", "", Bd_M, *mean_Kpipipi, *sigma2_Kpipipi);
    RooAddPdf * fit_Kpipipi = new RooAddPdf("fit_Kpipipi", "", RooArgList(*gauss1_Kpipipi, *gauss2_Kpipipi),
            RooArgList(*frac));

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
    std::ofstream params("../Results/combined_twoBody_fourBody.param");
    params << "mean_Kpi " << mean_Kpi->getVal() << " " << mean_Kpi->getError() << std::endl;
    params << "mean_Kpipipi " << mean_Kpipipi->getVal() << " " << mean_Kpipipi->getError() << std::endl;
    params << "sigma_Kpi " << sigma_Kpi->getVal() << " " << sigma_Kpi->getError() << std::endl;
    params << "sigma_ratio " << sigma_ratio->getVal() << " " << sigma_ratio->getError() << std::endl;
    params << "sigma_two_vs_four " << sigma_two_vs_four->getVal() << " " << sigma_two_vs_four->getError() << std::endl;
    params << "frac " << frac->getVal() << " " << frac->getError() << std::endl;
    params.close();

    // Convert PDFs to TH1s
    TH1F * h_data_two = (TH1F*)data_two->createHistogram("hData_two", Bd_M);
    TH1F * h_fit_two = (TH1F*)fit_Kpi->createHistogram("hFit_two", Bd_M, 
            RooFit::Binning(nBins * 10));
    TH1F * h_gauss1_two = (TH1F*)gauss1_Kpi->createHistogram("hGauss1_two", Bd_M, 
            RooFit::Binning(nBins * 10));
    TH1F * h_gauss2_two = (TH1F*)gauss2_Kpi->createHistogram("hGauss2_two", Bd_M, 
            RooFit::Binning(nBins * 10));
    TH1F * h_data_four = (TH1F*)data_four->createHistogram("hData_four", Bd_M);
    TH1F * h_fit_four = (TH1F*)fit_Kpipipi->createHistogram("hFit_four", Bd_M, 
            RooFit::Binning(nBins * 10));
    TH1F * h_gauss1_four = (TH1F*)gauss1_Kpipipi->createHistogram("hGauss1_four", Bd_M, 
            RooFit::Binning(nBins * 10));
    TH1F * h_gauss2_four = (TH1F*)gauss2_Kpipipi->createHistogram("hGauss2_four", Bd_M, 
            RooFit::Binning(nBins * 10));

    // Scale histograms
    h_fit_two->Scale(h_data_two->Integral() * 10);
    h_gauss1_two->Scale(h_data_two->Integral() * 10 * frac->getVal());
    h_gauss2_two->Scale(h_data_two->Integral() * 10 * (1 - frac->getVal()));
    h_fit_four->Scale(h_data_four->Integral() * 10);
    h_gauss1_four->Scale(h_data_four->Integral() * 10 * frac->getVal());
    h_gauss2_four->Scale(h_data_four->Integral() * 10 * (1 - frac->getVal()));

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
    TFile * outfile_two = TFile::Open("../Histograms/gaussian_Kpi.root", "RECREATE");
    outfile_two->cd();
    h_data_two->Write("data");
    h_fit_two->Write("fit");
    h_gauss1_two->Write("gauss1");
    h_gauss2_two->Write("gauss2");
    pulls_two->Write("pulls");
    outfile_two->Close();

    // Save four-body histograms
    TFile * outfile_four = TFile::Open("../Histograms/gaussian_Kpipipi.root", "RECREATE");
    outfile_four->cd();
    h_data_four->Write("data");
    h_fit_four->Write("fit");
    h_gauss1_four->Write("gauss1");
    h_gauss2_four->Write("gauss2");
    pulls_four->Write("pulls");
    outfile_four->Close();

    // Plot the results nicely
    Plotter * plotter = new Plotter("Kpi", "gaussian");
    plotter->plotFit("gauss1", "gauss2");
    delete plotter;
    Plotter * plotter_four = new Plotter("Kpipipi", "gaussian");
    plotter->plotFit("gauss1", "gauss2");
    delete plotter_four;

    // Display plot in TApplication
    //app->Run();
    
    return 0;
}
