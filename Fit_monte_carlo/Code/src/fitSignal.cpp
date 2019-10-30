#include <iostream>
#include <fstream>

#include "TApplication.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TFile.h"
#include "TH1F.h"

#include "RooAbsPdf.h"
#include "RooAbsReal.h"
#include "RooAddPdf.h"
#include "RooArgList.h"
#include "RooArgSet.h"
#include "RooCBShape.h"
#include "RooDataSet.h"
#include "RooFitResult.h"
#include "RooFormulaVar.h"
#include "RooGaussian.h"
#include "RooGlobalFunc.h"
#include "RooHist.h"
#include "RooPlotable.h"
#include "RooPlot.h"
#include "RooRealVar.h"
#include "RooCruijff.h"

#include "Plotter.hpp"

// Program for fitting to signal MC with a double crystal ball shape
int main(int argc, char * argv[]) {

    // Make a TApplication to display output
    //TApplication * app = new TApplication("app", &argc, argv);

    // Get D0 decay mode
    bool no_dtf = false;
    bool use_gauss = false;
    bool use_cruijff = false;
    if (argc != 2) {
        if (argc == 3) {
            std::string opt = argv[2];
            if (opt == "--noDTF") {
                no_dtf = true;
            } else if (opt == "--doubleGauss") {
                use_gauss = true;
            } else if (opt == "--cruijff") {
                std::cout << "Fitting with Cruijff" << std::endl;
                use_cruijff = true;
            } else {
                std::cout << "Unrecognised option: " << opt << std::endl;
                return -1;
            }
        } else {
            std::cout << "Usage: ./FitSignal <D0-mode>" << std::endl;
            return -1;
        }
    }
    std::string mode = argv[1];
    bool is_twoBody = true;
    bool is_Bs = false;
    bool is_run1 = false;
    bool is_threeBody = false;
    if (mode == "Kpipipi" || mode == "pipipipi") is_twoBody = false;
    if (mode == "Bs") is_Bs = true;
    if (mode == "run1") is_run1 = true;
    if (mode == "LL" || mode == "DD") is_threeBody = true;

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
    } else if (is_threeBody) {
        for (std::string year : {"16", "17", "18"}) {
                tree->Add((path + "../../GGSZ/MC/truth_matched/bdt_applied/default_precuts/D2KsPiPi_"
                            + mode + "_" + year + "_Up.root").c_str());
                tree->Add((path + "../../GGSZ/MC/truth_matched/bdt_applied/default_precuts/D2KsPiPi_"
                            + mode + "_" + year + "_Down.root").c_str());
            }
    } else {
        (is_twoBody) ? path += "twoBody/" : path += "fourBody/";
        path += mode + "/";
        tree->Add((path + "2016_down/" + mode + "_selected.root").c_str());
        tree->Add((path + "2016_up/" + mode + "_selected.root").c_str());
        if (is_twoBody) {
            for (std::string year : {"2015", "2017", "2018"}) {
                tree->Add((path + year + "_down/" + mode + "_selected.root").c_str());
                tree->Add((path + year + "_up/" + mode + "_selected.root").c_str());
            }
        }
    }

    // Set variables
    // To do: add double mis-ID cut to ADS modes
    double mass_diff = (is_Bs) ? 90 : 0;
    TString varname = no_dtf ? "Bd_M" : "Bd_ConsD_MD";
    if (is_threeBody) varname = "Bu_constD0KSPV_M";
    RooRealVar Bd_M(varname, "", 5160 + mass_diff, 5400 + mass_diff, "MeV/c^{2}");

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
    RooRealVar * sigma_L;
    if (use_cruijff) {
        sigma_L = new RooRealVar("sigma_L", "", 9, 5, 15);
    } else {
        sigma_L = new RooRealVar("sigma_L", "", 12, 0, 50);
    }
    RooRealVar * frac = new RooRealVar("frac", "", 0.5, 0, 1);

    // Gaussian-specific
    RooRealVar * sigma_ratio = new RooRealVar("sigma_ratio", "", 1, 0, 10);
    RooAbsReal * sigma_R;
    sigma_R = new RooFormulaVar("sigma_R", "@0 * @1", RooArgList(*sigma_L, *sigma_ratio));

    // CB-specific
    RooRealVar * alpha_L = new RooRealVar("alpha_L", "", 1.5, 0, 3);
    RooRealVar * alpha_R; 
    if (use_cruijff) {
        alpha_R = new RooRealVar("alpha_R", "", 0.1, 0, 3);
    } else {
        alpha_R = new RooRealVar("alpha_R", "", -1.5, -3, -0.001);
    }
    RooRealVar * n_L = new RooRealVar("n_L", "", 6, 0, 10);
    RooRealVar * n_R = new RooRealVar("n_R", "", 6, 0, 10);

    // PDFs: Gaussian
    RooGaussian * gauss1 = new RooGaussian("gauss1", "", Bd_M, *mean, *sigma_L);
    RooGaussian * gauss2 = new RooGaussian("gauss2", "", Bd_M, *mean, *sigma_R);

    // PDFs: CB
    RooCBShape * signal_L = new RooCBShape("signal_L", "", Bd_M, *mean, *sigma_L,
            *alpha_L, *n_L);
    RooCBShape * signal_R = new RooCBShape("signal_R", "", Bd_M, *mean, *sigma_L,
            *alpha_R, *n_R);

    // Add together chosen PDFs
    RooAbsPdf * signal;
    if (use_gauss) {
        signal = new RooAddPdf("signal", "", RooArgList(*gauss1,
                    *gauss2), RooArgList(*frac));
    } else if (use_cruijff) {
        signal = new RooCruijff("signal", "", Bd_M, *mean, *sigma_L,
                *sigma_R, *alpha_L, *alpha_R);
    } else {
        signal = new RooAddPdf("signal", "", RooArgList(*signal_L,
                    *signal_R), RooArgList(*frac));
    }

    // Fit to the dataset
    std::cout << "Dataset entries: " << data->sumEntries() << std::endl;
    RooFitResult * r = signal->fitTo(*data, RooFit::Save(), RooFit::NumCPU(8, 2),
            RooFit::Optimize(false), RooFit::Offset(false),
            RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    r->Print("v");
    std::cout << "min NLL: " << r->minNll() << std::endl;

    // Save output to a file
    std::string extra = no_dtf ? "_noDTF" : "";
    if (use_gauss) extra = "_gaussian";
    if (use_cruijff) extra = "_cruijff";
    if (is_threeBody) extra += "_GGSZ";
    std::ofstream params("../Results/signal_" + mode + extra + ".param");
    params << "mean " << mean->getVal() << " " << mean->getError() << std::endl;
    params << "sigma_L " << sigma_L->getVal() << " " << sigma_L->getError() << std::endl;
    if (use_gauss) {
        params << "sigma_ratio " << sigma_ratio->getVal() << " " << sigma_ratio->getError() << std::endl;
        params << "frac " << frac->getVal() << " " << frac->getError() << std::endl;
    } else {
        params << "alpha_L " << alpha_L->getVal() << " " << alpha_L->getError() << std::endl;
        params << "alpha_R " << alpha_R->getVal() << " " << alpha_R->getError() << std::endl;
        if (use_cruijff) {
            params << "sigma_ratio " << sigma_ratio->getVal() << " " << ((RooRealVar*)sigma_ratio)->getError() << std::endl;
        } else {
            params << "n_L " << n_L->getVal() << " " << n_L->getError() << std::endl;
            params << "n_R " << n_R->getVal() << " " << n_R->getError() << std::endl;
            params << "frac " << frac->getVal() << " " << frac->getError() << std::endl;
        }
    }
    params.close();

    // Convert PDFs to TH1s
    TFile * outfile = TFile::Open(("../Histograms/signal" + extra + "_" + mode + ".root").c_str(), 
            "RECREATE");
    TH1F * h_data = (TH1F*)data->createHistogram("data", Bd_M);
    TH1F * h_fit = (TH1F*)signal->createHistogram("fit", Bd_M, 
            RooFit::Binning(nBins * 10));
    TH1F * h_CB_L = (TH1F*)signal_L->createHistogram("CB_L", Bd_M, 
            RooFit::Binning(nBins * 10));
    TH1F * h_CB_R = (TH1F*)signal_R->createHistogram("CB_R", Bd_M, 
            RooFit::Binning(nBins * 10));
    TH1F * h_gauss1 = (TH1F*)gauss1->createHistogram("gauss1", Bd_M, 
            RooFit::Binning(nBins * 10));
    TH1F * h_gauss2 = (TH1F*)gauss2->createHistogram("gauss2", Bd_M, 
            RooFit::Binning(nBins * 10));

    // Scale histograms
    h_fit->Scale(h_data->Integral() * 10);
    h_CB_L->Scale(h_data->Integral() * 10 * frac->getVal());
    h_CB_R->Scale(h_data->Integral() * 10 * (1 - frac->getVal()));
    h_gauss1->Scale(h_data->Integral() * 10 * frac->getVal());
    h_gauss2->Scale(h_data->Integral() * 10 * (1 - frac->getVal()));

    // Plot the fit and display it in TApplication
    RooPlot * frame = Bd_M.frame(RooFit::Title(""));
    data->plotOn(frame);
    signal->plotOn(frame);
    RooHist * pulls = frame->pullHist();

    // Save fit result to file
    outfile->cd();
    r->Write("fit_result");

    // Save histograms and pulls to file
    h_data->Write("data");
    h_fit->Write("fit");
    if (use_gauss) {
        h_gauss1->Write("gauss1");
        h_gauss2->Write("gauss2");
    } else if (!use_cruijff) {
        h_CB_L->Write("CB_L");
        h_CB_R->Write("CB_R");
    }
    pulls->Write("pulls");
    outfile->Close();

    // Plot the results nicely
    Plotter * plotter = new Plotter(mode, "signal" + extra);
    if (use_gauss) {
        plotter->plotFit("gauss1", "gauss2");
    } else if (!use_cruijff) {
        plotter->plotFit("CB_L", "CB_R");
    } else {
        plotter->plotFit();
    }

    // Display plot in TApplication
    //app->Run();
    
    return 0;
}
