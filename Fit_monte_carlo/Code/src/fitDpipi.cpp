#include <iostream>
#include <fstream>

#include "TApplication.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TChain.h"
#include "TH1F.h"

#include "RooStats/SPlot.h"
#include "RooCruijff.h"
#include "RooGlobalFunc.h"
#include "RooExponential.h"
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
#include "RooGaussian.h"

#include "Plotter.hpp"

// Fit to misID invariant mass with a Gaussian
void fit_misID_gauss(int n_gauss = 1) {

    // Load Dpipi data
    std::string path = "/data/lhcb/users/pullen/B02DKstar/MC/backgrounds/Dpipi/";
    TChain *  tree = new TChain("DecayTree");
    tree->Add((path + "2016_down/Kpi_selected_withMisIDmass.root").c_str());
    tree->Add((path + "2016_up/Kpi_selected_withMisIDmass.root").c_str());

    // Set variables
    // To do: add double mis-ID cut to ADS modes
    RooRealVar Bd_M("misID_M", "", 5250, 5800, "MeV/c^{2}");
    RooRealVar weight("weight", "", 0, 1); // combination of sweight and PID efficiency

    // Set up bins
    int binWidth = 5;
    double nBins = (Bd_M.getMax() - Bd_M.getMin()) / binWidth;
    Bd_M.setBins(nBins);

    // Make args and dataset
    RooArgList args;
    args.add(Bd_M);
    args.add(weight);
    RooDataSet * data = new RooDataSet("data", "", tree, args, 0, "weight");

    // Make Gaussians
    std::map<int, RooRealVar*> means;
    std::map<int, RooRealVar*> sigmas;
    std::map<int, RooRealVar*> fracs;
    std::map<int, RooGaussian*> gauss;
    RooArgSet pdfs;
    RooArgSet f;
    int mean_shift = 0;
    int sigma_shift = 0;
    for (int i = 0; i < n_gauss; i++) {
        std::string i_str = std::to_string(i+1);
        means[i] = new RooRealVar(("mean"+i_str).c_str(), "", 5320 + mean_shift,
                5200 + mean_shift, 5500 + mean_shift);
        sigmas[i] = new RooRealVar(("sigma"+i_str).c_str(), "", 20 + sigma_shift,
                10 + sigma_shift, 30 + sigma_shift);
        gauss[i] = new RooGaussian(("gauss"+i_str).c_str(), "", Bd_M, *means[i],
                *sigmas[i]);
        pdfs.add(*gauss[i]);
        if (i < n_gauss - 1) {
            fracs[i] = new RooRealVar(("frac"+i_str).c_str(), "", 1.0/n_gauss,
                    0, 0.5);
            f.add(*fracs[i]);
        }
        mean_shift += 5;
        sigma_shift += 5;
    }

    // Add PDFs
    RooAddPdf * pdf = new RooAddPdf("signal", "", pdfs, f);

    // Fit to the dataset
    std::cout << "Dataset entries: " << data->sumEntries() << std::endl;
    RooFitResult * r = pdf->fitTo(*data, RooFit::Save(), RooFit::NumCPU(8, 2),
            RooFit::Optimize(false), RooFit::Offset(true), 
            RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    r->Print("v");

    // Save output to a file
    TString name = (std::to_string(n_gauss) + "_gaussians").c_str();
    std::ofstream params("../Results/Dpipi_" + name + ".param");
    for (int i = 0; i < n_gauss; i++) {
        params << "mean" << i+1 << " " << means[i]->getVal() << " " << means[i]->getError() << std::endl;
        params << "sigma" << i+1 << " " << sigmas[i]->getVal() << " " << sigmas[i]->getError() << std::endl;
        if (i != n_gauss - 1) {
            params << "frac" << i+1 << " " << fracs[i]->getVal() << " " << fracs[i]->getError() << std::endl;
        }
    }
    params.close();

    // Convert PDFs to TH1s
    TFile * outfile = TFile::Open("../Histograms/Dpipi_" + name + "_Kpi.root", "RECREATE");
    TH1F * h_data = (TH1F*)data->createHistogram("data", Bd_M);
    TH1F * h_fit = (TH1F*)pdf->createHistogram("fit", Bd_M, 
            RooFit::Binning(nBins * 10));
    h_fit->Scale(h_data->Integral() * 10);

    // Get pulls
    RooPlot * frame = Bd_M.frame(RooFit::Title(""));
    data->plotOn(frame);
    pdf->plotOn(frame);
    RooHist * pulls = frame->pullHist();

    // Save histograms and pulls to file
    outfile->cd();
    h_data->Write("data");
    h_fit->Write("fit");
    pulls->Write("pulls");
    r->Write("fit_result");

    // Write each Gaussian
    std::map<int, TH1F*> hists;
    std::vector<std::string> hists_to_plot;
    double sum_fracs = 0;
    for (int i = 0; i < n_gauss; i++) {
        hists[i] = (TH1F*)gauss[i]->createHistogram("h_" + *gauss[i]->GetName(), Bd_M,
            RooFit::Binning(nBins * 10));
        if (i < n_gauss -1) {
            hists[i]->Scale(h_data->Integral() * 10 * fracs[i]->getVal());
            sum_fracs += fracs[i]->getVal();
        } else {
            hists[i]->Scale(h_data->Integral() * 10 * (1 - sum_fracs));
        }
        hists[i]->Write(gauss[i]->GetName());
        hists_to_plot.push_back(gauss[i]->GetName());
    }
    outfile->Close();

    // Plot the results nicely
    Plotter * plotter = new Plotter("Kpi", "Dpipi_" + std::string(name));
    if (n_gauss == 1) {
        plotter->plotFit();
    } else {
        plotter->plotFit(hists_to_plot);
    }

}


// Fit to misID version of invariant mass
void fit_misID(bool use_rho_pars = false) {

    // Load Dpipi data
    std::string path = "/data/lhcb/users/pullen/B02DKstar/MC/backgrounds/Dpipi/";
    TChain *  tree = new TChain("DecayTree");
    tree->Add((path + "2016_down/Kpi_selected_withMisIDmass.root").c_str());
    tree->Add((path + "2016_up/Kpi_selected_withMisIDmass.root").c_str());

    // Set variables
    // To do: add double mis-ID cut to ADS modes
    RooRealVar Bd_M("misID_M", "", 5250, 5800, "MeV/c^{2}");
    // RooRealVar KstarK_PIDK("KstarK_PIDK", "", 5, 1000000);
    RooRealVar weight("weight", "", 0, 1); // combination of sweight and PID efficiency

    // Set up bins
    int binWidth = 5;
    double nBins = (Bd_M.getMax() - Bd_M.getMin()) / binWidth;
    Bd_M.setBins(nBins);

    // Make args and dataset
    RooArgList args;
    args.add(Bd_M);
    args.add(weight);
    RooDataSet * data = new RooDataSet("data", "", tree, args, 0, "weight");

    // Fit parameters
    RooRealVar *mean, *sigma_L, *sigma_ratio, *alpha_L, *alpha_R, *n_L, *n_R, *frac;
    if (!use_rho_pars) {
        mean = new RooRealVar("mean", "", 5320, 5200, 5500);
        sigma_L = new RooRealVar("sigma_L", "", 25, 0, 50);
        sigma_ratio = new RooRealVar("sigma_ratio", "", 1, 0.5, 5);
        alpha_L = new RooRealVar("alpha_L", "", 1.5, 0.5, 10);
        alpha_R = new RooRealVar("alpha_R", "", -0.3, -5, -0.001);
        n_L = new RooRealVar("n_L", "", 2, 0, 10);
        n_R = new RooRealVar("n_R", "", 9, 0, 10);
        frac = new RooRealVar("frac", "", 0.5, 0, 1);
    } else {
        mean = new RooRealVar("mean", "", 5320, 5200, 5500);
        sigma_L = new RooRealVar("sigma_L", "", 20.3043);
        sigma_ratio = new RooRealVar("sigma_ratio", "", 0.932881);
        alpha_L = new RooRealVar("alpha_L", "", 2.06895);
        alpha_R = new RooRealVar("alpha_R", "", -0.75314);
        n_L = new RooRealVar("n_L", "", 1);
        n_R = new RooRealVar("n_R", "", 4.77975);
        frac = new RooRealVar("frac", "", 0.323632);
    }
    RooFormulaVar * sigma_R = new RooFormulaVar("sigma_R", "@0 * @1",
        RooArgList(*sigma_L, *sigma_ratio));

    // PDFs
    RooCBShape * signal_L = new RooCBShape("signal_L", "", Bd_M, *mean, *sigma_L,
            *alpha_L, *n_L);
    RooCBShape * signal_R = new RooCBShape("signal_R", "", Bd_M, *mean, *sigma_R,
            *alpha_R, *n_R);
    RooAddPdf * pdf = new RooAddPdf("signal", "", RooArgList(*signal_L, 
                *signal_R), RooArgList(*frac));

    // Make exponential for background
    // RooRealVar * slope = new RooRealVar("slope", "", -0.005, -0.007, 0.0);
    // RooExponential * expo = new RooExponential("expo", "", Bd_M, *slope);

    // Combine
    // RooRealVar * f_signal = new RooRealVar("f_signal", "", 0.8, 0, 1);
    // RooAddPdf * pdf = new RooAddPdf("pdf", "", RooArgList(*signal, *expo),
            // RooArgList(*f_signal));

    // Fit to the dataset
    std::cout << "Dataset entries: " << data->sumEntries() << std::endl;
    RooFitResult * r = pdf->fitTo(*data, RooFit::Save(), RooFit::NumCPU(8, 2),
            RooFit::Optimize(false), RooFit::Offset(true), 
            RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    r->Print("v");

    // Save output to a file
    TString extra = use_rho_pars ? "_rhoPars" : "";
    std::ofstream params("../Results/Dpipi" + extra + ".param");
    params << "alpha_L " << alpha_L->getVal() << " " << alpha_L->getError() << std::endl;
    params << "alpha_R " << alpha_R->getVal() << " " << alpha_R->getError() << std::endl;
    params << "frac " << frac->getVal() << " " << frac->getError() << std::endl;
    params << "mean " << mean->getVal() << " " << mean->getError() << std::endl;
    params << "n_L " << n_L->getVal() << " " << n_L->getError() << std::endl;
    params << "n_R " << n_R->getVal() << " " << n_R->getError() << std::endl;
    params << "sigma_L " << sigma_L->getVal() << " " << sigma_L->getError() << std::endl;
    params << "sigma_ratio " << sigma_ratio->getVal() << " " << sigma_ratio->getError() << std::endl;
    // params << "slope" << slope->getVal() << " " << slope->getError() << std::endl;
    // params << "f_signal" << f_signal->getVal() << " " << f_signal->getError() << std::endl;
    params.close();

    // Convert PDFs to TH1s
    TFile * outfile = TFile::Open("../Histograms/Dpipi" + extra + "_Kpi.root", "RECREATE");
    TH1F * h_data = (TH1F*)data->createHistogram("data", Bd_M);
    TH1F * h_fit = (TH1F*)pdf->createHistogram("fit", Bd_M, 
            RooFit::Binning(nBins * 10));
    // TH1F * h_signal = (TH1F*)signal->createHistogram("signal", Bd_M,
            // RooFit::Binning(nBins * 10));
    // TH1F * h_expo = (TH1F*)expo->createHistogram("comb", Bd_M,
            // RooFit::Binning(nBins * 10));
    TH1F * h_CB_L = (TH1F*)signal_L->createHistogram("CB_L", Bd_M,
            RooFit::Binning(nBins * 10));
    TH1F * h_CB_R = (TH1F*)signal_R->createHistogram("CB_R", Bd_M,
            RooFit::Binning(nBins * 10));

    // Scale histograms
    h_fit->Scale(h_data->Integral() * 10);
    // h_signal->Scale(h_data->Integral() * 10 * f_signal->getVal());
    // h_expo->Scale(h_data->Integral() * 10 * (1 - f_signal->getVal()));
    h_CB_L->Scale(h_data->Integral() * 10 * frac->getVal());
    h_CB_R->Scale(h_data->Integral() * 10 * (1 - frac->getVal()));

    // Get pulls
    RooPlot * frame = Bd_M.frame(RooFit::Title(""));
    data->plotOn(frame);
    pdf->plotOn(frame);
    RooHist * pulls = frame->pullHist();

    // Save histograms and pulls to file
    outfile->cd();
    h_data->Write("data");
    h_fit->Write("fit");
    // h_signal->Write("signal");
    // h_expo->Write("comb");
    h_CB_L->Write("CB_L");
    h_CB_R->Write("CB_R");
    pulls->Write("pulls");
    r->Write("fit_result");
    outfile->Close();

    // Plot the results nicely
    Plotter * plotter = new Plotter("Kpi", "Dpipi" + std::string(extra));
    // plotter->plotFit("signal", "comb");
    plotter->plotFit("CB_L", "CB_R");
    // plotter->plotFit();

}

// Fit to signal and apply sWeights
void fit_signal(TString mag) {

    // Load data
    TString path = "/data/lhcb/users/pullen/B02DKstar/MC/backgrounds/Dpipi/";
    TChain *  tree = new TChain("DecayTree");
    tree->Add(path + "2016_" + mag + "/Kpi_selected_withMisIDmass.root");
    RooRealVar Bd_M("B_M", "", 5200, 5500, "MeV/c^{2}");
    // RooRealVar Bd_M("B_M", "", 5200, 5500, "MeV/c^{2}");

    // Set up bins
    int binWidth = 5;
    double nBins = (Bd_M.getMax() - Bd_M.getMin()) / binWidth;
    Bd_M.setBins(nBins);

    // Make args and dataset
    RooArgList args;
    args.add(Bd_M);
    RooDataSet * data = new RooDataSet("data", "", tree, args);

    // Make signal shape
    RooRealVar * mean = new RooRealVar("mean", "", 5280, 5200, 5400);
    RooRealVar * sigma_L = new RooRealVar("sigma_L", "", 9, 5, 15);
    RooRealVar * sigma_ratio = new RooRealVar("sigma_ratio", "", 1, 0, 10);
    RooFormulaVar * sigma_R = new RooFormulaVar("sigma_R", "@0 * @1", 
            RooArgList(*sigma_L, *sigma_ratio));
    RooRealVar * alpha_L = new RooRealVar("alpha_L", "", 1.5, 0, 3);
    RooRealVar * alpha_R = new RooRealVar("alpha_R", "", 0.1, 0, 3);
    RooRealVar * n_L = new RooRealVar("n_L", "", 6, 0, 10);
    RooRealVar * n_R = new RooRealVar("n_R", "", 6, 0, 10);
    RooCruijff * signal = new RooCruijff("signal", "", Bd_M, *mean, *sigma_L,
                *sigma_R, *alpha_L, *alpha_R);

    // Make exponential for background
    RooRealVar * slope = new RooRealVar("slope", "", -0.005, -0.007, 0.0);
    RooExponential * expo = new RooExponential("expo", "", Bd_M, *slope);

    // Combine
    double entries = tree->GetEntries();
    RooRealVar * N_signal = new RooRealVar("N_signal", "", 0.8*entries, 0, entries);
    RooRealVar * N_expo = new RooRealVar("N_expo", "", 0.2*entries, 0, entries);
    RooAddPdf * pdf = new RooAddPdf("pdf", "", RooArgList(*signal, *expo),
            RooArgList(*N_signal, *N_expo));

    // Perform the fit
    RooFitResult * r = pdf->fitTo(*data, RooFit::Save(), RooFit::NumCPU(8, 2),
            RooFit::Optimize(false), RooFit::Offset(true), 
            RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    r->Print("v");

    // Plot fit
    RooPlot * plt = Bd_M.frame();
    data->plotOn(plt, RooFit::DrawOption("PZ"), RooFit::LineWidth(1), 
            RooFit::LineColor(kBlack));
    pdf->plotOn(plt, RooFit::LineColor(kBlack), RooFit::DrawOption("C"), 
            RooFit::LineWidth(2), RooFit::ProjWData(*data));
    pdf->plotOn(plt, RooFit::LineColor(kBlue), RooFit::DrawOption("C"), 
            RooFit::LineWidth(2), RooFit::ProjWData(*data), 
            RooFit::Components("expo"));
    pdf->plotOn(plt, RooFit::LineColor(kRed + 2), RooFit::DrawOption("C"), 
            RooFit::LineWidth(2), RooFit::ProjWData(*data), 
            RooFit::Components("signal"));
    TCanvas * canvas = new TCanvas("canv", "", 900, 600);
    plt->Draw();
    canvas->SaveAs("../Plots/Dpipi_signal_" + mag + ".pdf");

    // Set non-yield params constant
    mean->setConstant();
    sigma_L->setConstant();
    alpha_L->setConstant();
    alpha_R->setConstant();
    n_L->setConstant();
    n_R->setConstant();
    slope->setConstant(); 

    // Get sWeights
    RooStats::SPlot * sData = new RooStats::SPlot("sData", "", *data, pdf, 
            RooArgList(*N_signal, *N_expo));
    sData->GetSWeightVars().Print();

    // Make output tree
    TFile * outfile = new TFile("/data/lhcb/users/pullen/B02DKstar/MC/"
            "backgrounds/Dpipi/2016_" + mag + "/Kpi_selected_sWeights.root",
            "RECREATE");
    TTree * outtree = tree->CloneTree(0);
    Double_t sw_signal, weight;
    outtree->Branch("sw_signal", &sw_signal, "sw_signal/D");
    outtree->Branch("weight", &weight, "weight/D");
    double mass;
    float PID_eff;
    tree->SetBranchAddress("B_M", &mass);
    tree->SetBranchAddress("PID_efficiency", &PID_eff);

    int nentries = tree->GetEntries();
    int n_in_range = 0;
    for (Long64_t n_e = 0; n_e < nentries; n_e++) {
        Long64_t entry = tree->GetEntryNumber(n_e);
        tree->GetEntry(entry);
        // Check mass is in range
        if (5200 < mass && mass < 5500){
            // Extract sWeights
            sw_signal = sData->GetSWeight(n_in_range, "N_signal");
            weight = sw_signal * PID_eff;
            n_in_range++;
            outtree->Fill();
        }
    }

    // Save
    outtree->Write("DecayTree");
    outfile->Close();

}



// Program for fitting to Dpipi data 
int main(int argc, char * argv[]) {

    bool use_rho_pars = false;
    if (argc > 1) {
        std::string opt = argv[1];
        if (opt == "--sWeight") {
            if (argc == 2) {
                std::cout << "Usage: ./FitDpipi --sWeight <up/down>" << std::endl;
                return -1;
            }
            std::cout << "Fitting to signal and applying sWeights..." << std::endl;
            fit_signal(argv[2]);
        } else if (opt == "--rho") {
            std::cout << "Using fixed rho parameters in fit..." << std::endl;
            use_rho_pars = true;
        } else if (opt == "--gauss") {
            int n_gauss = 1;
            if (argc > 2) n_gauss = atoi(argv[2]);
            std::cout << "Fitting with " << n_gauss << " Gaussians" << std::endl;
            fit_misID_gauss(n_gauss);
        } else {
            std::cout << "Unrecognised option " << opt << std::endl;
            return -1;
        }
    } 
    if (argc == 1 || use_rho_pars) {
        std::cout << "Fitting to mis ID mass..." << std::endl;
        fit_misID(use_rho_pars);
    }
    
    return 0;
}
