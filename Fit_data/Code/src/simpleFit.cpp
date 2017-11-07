#include <iostream>
#include <fstream>
#include <vector>

#include "TApplication.h"
#include "TChain.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH1F.h"

#include "RooAddPdf.h"
#include "RooArgList.h"
#include "RooArgSet.h"
#include "RooCategory.h"
#include "RooCBShape.h"
#include "RooDataSet.h"
#include "RooExponential.h"
#include "RooFitResult.h"
#include "RooFormulaVar.h"
#include "RooPlot.h"
#include "RooRealVar.h"
#include "RooSimultaneous.h"
#include "RooUnblindUniform.h"
#include "RooHist.h"

#include "RooHORNSdini.h"
#include "RooHILLdini.h"
#include "RooLITTLEHORNSdini.h"

#include "ParameterReader.hpp"
#include "Plotter.hpp"

int main(int argc, char * argv[]) {

    // ===============
    // Read input args
    // ===============
    // Check for parameters
    if (argc != 3) {
        std::cout << "Usage: ./FitTwoBody <2011:2012:2015:2016> <Sum: Y/N>"
            << std::endl;
        return -1;
    }

    // Year (colon separated string)
    std::string input_year = argv[1];

    // Sum over charge (Y/N)
    std::string sum = argv[2];

    // Vectors of years and D0 modes
    std::vector<std::string> years = {"2011", "2012", "2015", "2016"};
    std::vector<std::string> modes = {"Kpi", "piK"};
    //std::vector<std::string> modes = {"Kpi", "piK", "KK", "pipi"};

    // ================
    // Set up variables
    // ================
    // Common parameters
    RooRealVar Bd_M("Bd_ConsD_MD", "", 5000, 5800);
    RooRealVar KstarK_ID("KstarK_ID", "", -100000000, 100000000);
    RooRealVar D0_FDS("D0_FDS", "", 2, 100000000);
    RooRealVar D0_M("D0_M", "", 1864.83 - 25, 1864.83 + 25);

    // BDT response
    RooRealVar BDT_Kpi("BDTG_Kpi_run2", "", 0.5, 1);

    // PID variables
    RooRealVar KstarK_PIDK("KstarK_PIDK", "", 3, 100000000);
    RooRealVar KstarPi_PIDK("KstarPi_PIDK", "", -100000000, -1);
    RooRealVar D0K_PIDK("D0K_PIDK", "", 1, 100000000);
    RooRealVar D0Pi_PIDK("D0Pi_PIDK", "", -100000000, -1);

    // Make list of args to use
    RooArgList args;
    args.add(Bd_M);
    args.add(KstarK_ID);
    args.add(D0_FDS);
    args.add(D0_M);
    args.add(BDT_Kpi);
    args.add(KstarK_PIDK);
    args.add(KstarPi_PIDK);
    args.add(D0K_PIDK);
    args.add(D0Pi_PIDK);

    // Set up bins
    int binWidth = 5;
    double nBins = (Bd_M.getMax() - Bd_M.getMin()) / binWidth;
    Bd_M.setBins(nBins);

    // ============
    // Read in data
    // ============
    std::string path = "/data/lhcb/users/pullen/B02DKstar/data/twoBody/";
    std::map<std::string, RooDataSet *> data_both;
    std::map<std::string, RooDataSet *> data_plus;
    std::map<std::string, RooDataSet *> data_minus;

    // Loop through modes
    for (auto mode : modes) {

        // Make TChain to hold data
        TChain * chain = new TChain("DecayTree");

        // Loop through years
        for (auto year : years) {
            if (input_year.find(year) != std::string::npos) {
                std::string filepath_down = path + year + "_down/" + mode + "_withVars_withCuts.root";
                std::string filepath_up = path + year + "_up/" + mode + "_withVars_withCuts.root";
                chain->Add(filepath_down.c_str());
                chain->Add(filepath_up.c_str());
            }
        }

        // Turn off irrelevant branches
        chain->SetBranchStatus("*", 0);
        chain->SetBranchStatus("Bd_ConsD_MD", 1);
        chain->SetBranchStatus("KstarK_ID", 1);     
        chain->SetBranchStatus("D0_FDS", 1);     
        chain->SetBranchStatus("D0_M", 1);
        chain->SetBranchStatus("BDTG_Kpi_run2", 1);
        chain->SetBranchStatus("KstarK_PIDK", 1);
        chain->SetBranchStatus("KstarPi_PIDK", 1);
        chain->SetBranchStatus("D0K_PIDK", 1);
        chain->SetBranchStatus("D0Pi_PIDK", 1);

        // Convert to RooDataSet
        std::cout << "Making RooDataSet for mode: " << mode << std::endl;
        data_both[mode] = new RooDataSet(("data_" + mode).c_str(), "", chain, args);
        data_both[mode]->Print();
    }

    // =========================
    // Read in MC fit parameters
    // =========================
    ParameterReader pr;
    std::string MC_path = "/home/pullen/analysis/B02DKstar/Fit_monte_carlo/Results/";
    pr.readParams("signal", MC_path + "signal_Kpi.param");
    pr.readParams("Bs", MC_path + "signal_Bs.param");
    pr.readParams("low", MC_path + "lowMass.param");

    // =================
    // Make signal shape
    // =================
    // Shifts
    RooRealVar * delta_M = new RooRealVar("delta_M", "", 87.26, 80, 90);
    RooRealVar * shift = new RooRealVar("shift", "", 0, -10, 10);

    // Set up parameters
    RooRealVar * signal_mean_fixed = new RooRealVar("signal_mean_fixed", "", pr["signal_mean"]); 
    RooFormulaVar * signal_mean = new RooFormulaVar("signal_mean", "@0 + @1", RooArgList(*signal_mean_fixed, *shift));
    RooRealVar * signal_sigma_L = new RooRealVar("signal_sigma_L", "", pr["signal_sigma_L"]);
    RooRealVar * signal_sigma_R = new RooRealVar("signal_sigma_R", "", pr["signal_sigma_R"]);
    RooRealVar * signal_alpha_L = new RooRealVar("signal_alpha_L", "", pr["signal_alpha_L"]);
    RooRealVar * signal_alpha_R = new RooRealVar("signal_alpha_R", "", pr["signal_alpha_R"]);
    RooRealVar * signal_n_L = new RooRealVar("signal_n_L", "", pr["signal_n_L"]);
    RooRealVar * signal_n_R = new RooRealVar("signal_n_R", "", pr["signal_n_R"]);
    RooRealVar * signal_frac = new RooRealVar("signal_frac", "", pr["signal_frac"]);

    // Make crystal ball PDFs
    RooCBShape * signal_CB_L = new RooCBShape("signal_CB_L", "", Bd_M, *signal_mean, 
            *signal_sigma_L, *signal_alpha_L, *signal_n_L);
    RooCBShape * signal_CB_R = new RooCBShape("signal_CB_R", "", Bd_M, *signal_mean, 
            *signal_sigma_R, *signal_alpha_R, *signal_n_R);
    RooAddPdf * signal_shape = new RooAddPdf("signal_shape", "", 
            RooArgList(*signal_CB_L, *signal_CB_R), RooArgList(*signal_frac));

    // =============
    // Make Bs shape
    // =============
    // Set up parameters
    RooFormulaVar * Bs_mean = new RooFormulaVar("Bs_mean", "@0 + @1", RooArgList(*signal_mean, *delta_M));
    RooRealVar * Bs_sigma_L = new RooRealVar("Bs_sigma_L", "", pr["Bs_sigma_L"]);
    RooRealVar * Bs_sigma_R = new RooRealVar("Bs_sigma_R", "", pr["Bs_sigma_R"]);
    RooRealVar * Bs_alpha_L = new RooRealVar("Bs_alpha_L", "", pr["Bs_alpha_L"]);
    RooRealVar * Bs_alpha_R = new RooRealVar("Bs_alpha_R", "", pr["Bs_alpha_R"]);
    RooRealVar * Bs_n_L = new RooRealVar("Bs_n_L", "", pr["Bs_n_L"]);
    RooRealVar * Bs_n_R = new RooRealVar("Bs_n_R", "", pr["Bs_n_R"]);
    RooRealVar * Bs_frac = new RooRealVar("Bs_frac", "", pr["Bs_frac"]);

    // Make crystal ball PDFs
    RooCBShape * Bs_CB_L = new RooCBShape("Bs_CB_L", "", Bd_M, *Bs_mean, *Bs_sigma_L, *Bs_alpha_L, *Bs_n_L);
    RooCBShape * Bs_CB_R = new RooCBShape("Bs_CB_R", "", Bd_M, *Bs_mean, *Bs_sigma_R, *Bs_alpha_R, *Bs_n_R);
    RooAddPdf * Bs_shape = new RooAddPdf("Bs_shape", "", RooArgList(*Bs_CB_L, *Bs_CB_R), RooArgList(*Bs_frac));

    // ====================
    // Make low mass shapes
    // ====================
    // Kinematic endpoints
    RooRealVar * low_a_gamma = new RooRealVar("low_a_gamma", "", pr["low_a_gamma"]);
    RooRealVar * low_b_gamma = new RooRealVar("low_b_gamma", "", pr["low_b_gamma"]);
    RooRealVar * low_a_pi = new RooRealVar("low_a_pi", "", pr["low_a_pi"]);
    RooRealVar * low_b_pi = new RooRealVar("low_b_pi", "", pr["low_b_pi"]);

    // Widths
    RooRealVar * low_sigma_gamma_010 = new RooRealVar("low_sigma_gamma_010", "", pr["low_sigma_gamma_010"]);
    RooRealVar * low_sigma_gamma_101 = new RooRealVar("low_sigma_gamma_101", "", pr["low_sigma_gamma_101"]);
    RooRealVar * low_sigma_pi_010 = new RooRealVar("low_sigma_pi_010", "", pr["low_sigma_pi_010"]);
    RooRealVar * low_sigma_pi_101 = new RooRealVar("low_sigma_pi_101", "", pr["low_sigma_pi_101"]);

    // Csi
    RooRealVar * low_csi_gamma_010 = new RooRealVar("low_csi_gamma_010", "", pr["low_csi_gamma_010"]);
    RooRealVar * low_csi_gamma_101 = new RooRealVar("low_csi_gamma_101", "", pr["low_csi_gamma_101"]);
    RooRealVar * low_csi_pi_010 = new RooRealVar("low_csi_pi_010", "", pr["low_csi_pi_010"]);
    RooRealVar * low_csi_pi_101 = new RooRealVar("low_csi_pi_101", "", pr["low_csi_pi_101"]);

    // Fraction and ratio of Gaussians
    RooRealVar * low_frac = new RooRealVar("low_frac", "", pr["low_frac"]);
    RooRealVar * low_ratio = new RooRealVar("low_ratio", "", pr["low_ratio"]);
    RooRealVar * low_shiftg = new RooRealVar("low_shiftg", "", 0);

    // Make shapes
    RooHILLdini * gamma_010_shape = new RooHILLdini("gamma_010_shape", "", Bd_M, 
            *low_a_gamma, *low_b_gamma, *low_csi_gamma_010, *shift, 
            *low_sigma_gamma_010, *low_ratio, *low_frac);
    RooLITTLEHORNSdini * gamma_101_shape = new RooLITTLEHORNSdini("gamma_101_shape", "", Bd_M, 
            *low_a_gamma, *low_b_gamma, *low_csi_gamma_101, *shift, 
            *low_sigma_gamma_101, *low_ratio, *low_frac, *low_shiftg);
    RooHORNSdini * pi_010_shape = new RooHORNSdini("pi_010_shape", "", Bd_M, 
            *low_a_pi, *low_b_pi, *low_csi_pi_010, *shift, 
            *low_sigma_pi_010, *low_ratio, *low_frac);
    RooHILLdini * pi_101_shape = new RooHILLdini("pi_101_shape", "", Bd_M, 
            *low_a_pi, *low_b_pi, *low_csi_pi_101, *shift, 
            *low_sigma_pi_101, *low_ratio, *low_frac);

    // =======================
    // Make Bs low mass shapes
    // =======================
    // Shifted endpoints
    RooFormulaVar * Bs_low_a_gamma = new RooFormulaVar("Bs_low_a_gamma", "@0 + @1", RooArgList(*low_a_gamma, *delta_M));
    RooFormulaVar * Bs_low_b_gamma = new RooFormulaVar("Bs_low_b_gamma", "@0 + @1", RooArgList(*low_b_gamma, *delta_M));
    RooFormulaVar * Bs_low_a_pi = new RooFormulaVar("Bs_low_a_pi", "@0 + @1", RooArgList(*low_a_pi, *delta_M));
    RooFormulaVar * Bs_low_b_pi = new RooFormulaVar("Bs_low_b_pi", "@0 + @1", RooArgList(*low_b_pi, *delta_M));

    // Widths
    RooRealVar * Bs_low_sigma_gamma_010 = new RooRealVar("Bs_low_sigma_gamma_010", "", pr["low_Bs_sigma_gamma_010"]);
    RooRealVar * Bs_low_sigma_gamma_101 = new RooRealVar("Bs_low_sigma_gamma_101", "", pr["low_Bs_sigma_gamma_101"]);
    RooRealVar * Bs_low_sigma_pi_010 = new RooRealVar("Bs_low_sigma_pi_010", "", pr["low_Bs_sigma_pi_010"]);
    RooRealVar * Bs_low_sigma_pi_101 = new RooRealVar("Bs_low_sigma_pi_101", "", pr["low_Bs_sigma_pi_101"]);

    // Csi
    RooRealVar * Bs_low_csi_gamma_010 = new RooRealVar("Bs_low_csi_gamma_010", "", pr["low_Bs_csi_gamma_010"]);
    RooRealVar * Bs_low_csi_gamma_101 = new RooRealVar("Bs_low_csi_gamma_101", "", pr["low_Bs_csi_gamma_101"]);
    RooRealVar * Bs_low_csi_pi_010 = new RooRealVar("Bs_low_csi_pi_010", "", pr["low_Bs_csi_pi_010"]);
    RooRealVar * Bs_low_csi_pi_101 = new RooRealVar("Bs_low_csi_pi_101", "", pr["low_Bs_csi_pi_101"]);

    // Make shapes
    RooHILLdini * Bs_gamma_010_shape = new RooHILLdini("Bs_gamma_010_shape", "", Bd_M, 
            *Bs_low_a_gamma, *Bs_low_b_gamma, *Bs_low_csi_gamma_010, *shift, 
            *Bs_low_sigma_gamma_010, *low_ratio, *low_frac);
    RooLITTLEHORNSdini * Bs_gamma_101_shape = new RooLITTLEHORNSdini("Bs_gamma_101_shape", "", Bd_M, 
            *Bs_low_a_gamma, *Bs_low_b_gamma, *Bs_low_csi_gamma_101, *shift, 
            *Bs_low_sigma_gamma_101, *low_ratio, *low_frac, *shift);
    RooHORNSdini * Bs_pi_010_shape = new RooHORNSdini("Bs_pi_010_shape", "", Bd_M, 
            *Bs_low_a_pi, *Bs_low_b_pi, *Bs_low_csi_pi_010, *shift, 
            *Bs_low_sigma_pi_010, *low_ratio, *low_frac);
    RooHILLdini * Bs_pi_101_shape = new RooHILLdini("Bs_pi_101_shape", "", Bd_M, 
            *Bs_low_a_pi, *Bs_low_b_pi, *Bs_low_csi_pi_101, *shift, 
            *Bs_low_sigma_pi_101, *low_ratio, *low_frac);

    // =======================
    // Combine low mass shapes
    // =======================
    // Calculate fractions of pi and gamma
    const double branching_gamma = 0.353;
    const double branching_pi = 0.647;
    const double acc_gamma = 17.48;
    const double acc_pi = 15.53;
    double G_010 = branching_gamma * acc_gamma * 0.009227;
    double G_101 = branching_gamma * acc_gamma * 0.004613; 
    double P_010 = branching_pi * acc_pi * 0.01168;
    double P_101 = branching_pi * acc_pi * 0.005712;
    
    // Make helicity fraction parameters
    RooRealVar * coeff_gamma_010 = new RooRealVar("coeff_gamma_010", "", G_010 / (P_010 + G_010));
    RooRealVar * coeff_gamma_101 = new RooRealVar("coeff_gamma_101", "", G_101 / (P_101 + G_101));
    RooRealVar * coeff_pi_010 = new RooRealVar("coeff_pi_010", "", P_010 / (P_010 + G_010));
    RooRealVar * coeff_pi_101 = new RooRealVar("coeff_pi_101", "", P_101 / (P_101 + G_101));

    // Combine helicity components
    RooAddPdf * low_010_shape = new RooAddPdf("low_010_shape", "", 
            RooArgList(*gamma_010_shape, *pi_010_shape), 
            RooArgList(*coeff_gamma_010, *coeff_pi_010));
    RooAddPdf * low_101_shape = new RooAddPdf("low_101_shape", "", 
            RooArgList(*gamma_101_shape, *pi_101_shape), 
            RooArgList(*coeff_gamma_101, *coeff_pi_101));
    RooAddPdf * Bs_low_010_shape = new RooAddPdf("Bs_low_010_shape", "", 
            RooArgList(*Bs_gamma_010_shape, *Bs_pi_010_shape), 
            RooArgList(*coeff_gamma_010, *coeff_pi_010));
    RooAddPdf * Bs_low_101_shape = new RooAddPdf("Bs_low_101_shape", "", 
            RooArgList(*Bs_gamma_101_shape, *Bs_pi_101_shape), 
            RooArgList(*coeff_gamma_101, *coeff_pi_101));

    // Make floating fraction of 010 shape
    RooRealVar * low_frac_010_Kpi = new RooRealVar("low_frac_010_Kpi", "", 0.5, 0, 1);
    RooRealVar * low_frac_010_piK = new RooRealVar("low_frac_010_piK", "", 0.5, 0, 1);
    RooRealVar * Bs_low_frac_010 = new RooRealVar("Bs_low_frac_010", "", 0.5, 0, 1);

    // Combine helicity components
    std::map<std::string, RooAddPdf*> low_shapes;
    low_shapes["Kpi"] = new RooAddPdf("low_Kpi_shape", "", 
            RooArgList(*low_010_shape, *low_101_shape), RooArgList(*low_frac_010_Kpi));
    low_shapes["piK"] = new RooAddPdf("low_piK_shape", "", 
            RooArgList(*low_010_shape, *low_101_shape), RooArgList(*low_frac_010_piK));
    RooAddPdf * Bs_low_shape = new RooAddPdf("Bs_low_shape", "", 
            RooArgList(*Bs_low_010_shape, *Bs_low_101_shape), RooArgList(*Bs_low_frac_010));


    // ========================
    // Make combinatorial shape
    // ========================
    std::map<std::string, RooExponential *> expo_shapes;
    RooRealVar * slope_Kpi = new RooRealVar("slope_Kpi", "", -0.005, -0.5, 0.0);
    expo_shapes["Kpi"] = new RooExponential("expo_Kpi_shape", "", Bd_M, *slope_Kpi);
    expo_shapes["piK"] = expo_shapes["Kpi"];

    // ==================
    // Set up observables
    // ==================
    // Signal ratios
    // Suppressed to favoured ADS ratio
    RooRealVar * R_piK_vs_Kpi_blind = new RooRealVar("R_piK_vs_Kpi_blind", "", 0.06, 0, 1);
    RooUnblindUniform * R_piK_vs_Kpi = new RooUnblindUniform("R_piK_vs_Kpi", "", "blind_piK_ratio", 0.01, *R_piK_vs_Kpi_blind);

    // Low mass ratios
    RooRealVar * R_piK_vs_Kpi_low = new RooRealVar("R_piK_vs_Kpi_low", "", 0.06, 0, 1);


    // =============
    // Set up yields
    // =============
    // Maps to hold yields for each mode
    std::map<std::string, std::map<std::string, RooAbsReal *>> yields;

    // Signal yields
    yields["Kpi"]["n_signal"] = new RooRealVar("n_signal_Kpi", "", 100, 0, 20000);
    yields["piK"]["n_signal"] = new RooFormulaVar("n_signal_piK", "@0 * @1", 
            RooArgList(*yields["Kpi"]["n_signal"], *R_piK_vs_Kpi));

    // Bs yields
    yields["piK"]["n_Bs"] = new RooRealVar("n_Bs_piK", "", 100, 0, 20000);

    // Low mass yields
    yields["Kpi"]["n_low"] = new RooRealVar("n_low_Kpi", "", 100, 0, 20000);
    yields["piK"]["n_low"] = new RooFormulaVar("n_low_piK", "@0 * @1", 
            RooArgList(*yields["Kpi"]["n_low"], *R_piK_vs_Kpi_low));

    // Bs low mass yields
    yields["piK"]["n_Bs_low"] = new RooRealVar("n_Bs_low_piK", "", 100, 0, 20000);

    // Exponential yields
    yields["Kpi"]["n_expo"] = new RooRealVar("n_expo_Kpi", "", 100, 0, 20000);
    yields["piK"]["n_expo"] = new RooRealVar("n_expo_piK", "", 100, 0, 20000);

    // ======================
    // Make overall fit shape
    // ======================
    // Set up categories
    RooCategory category("category", "");
    for (auto mode : modes) category.defineType(mode.c_str());

    // Make combined dataset
    RooDataSet combData("combData", "", Bd_M, RooFit::Index(category), RooFit::Import(data_both));

    // Make each total fit shape
    std::map<std::string, RooAddPdf *> fitShapes;
    for (auto mode : modes) {
        // Lists of shapes and yields
        RooArgList shapes_list;
        RooArgList yields_list;

        // Add shapes
        shapes_list.add(*signal_shape);
        shapes_list.add(*(expo_shapes[mode]));
        shapes_list.add(*(low_shapes[mode]));
        if (mode != "Kpi") {
            shapes_list.add(*Bs_shape);
            shapes_list.add(*Bs_low_shape);
        }

        // Add yields
        yields_list.add(*(yields[mode]["n_signal"]));
        yields_list.add(*(yields[mode]["n_expo"]));
        yields_list.add(*(yields[mode]["n_low"]));
        if (mode != "Kpi") {
            yields_list.add(*(yields[mode]["n_Bs"]));
            yields_list.add(*(yields[mode]["n_Bs_low"]));
        }

        // Make shape
        fitShapes[mode] = new RooAddPdf(("fit_" + mode).c_str(), "", shapes_list, yields_list);
    }

    // Combine the PDFs
    RooSimultaneous * simPdf = new RooSimultaneous("simPdf", "", category);
    for (auto mode : modes) simPdf->addPdf(*(fitShapes[mode]), mode.c_str());

    // ===========
    // Perform fit
    // ===========
    RooFitResult * r = simPdf->fitTo(combData, RooFit::Save(), RooFit::NumCPU(8, 2),
            RooFit::Optimize(false), RooFit::Offset(true), 
            RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    r->Print("v");

    // ================================
    // Save results to a histogram file
    // ================================
    // Open file
    TFile * outfile = TFile::Open("../Histograms/fits_twoBody.root", "RECREATE");

    // Loop through modes
    for (auto mode : modes) {
        // Convert data and fit to TH1Fs
        TH1F * h_data = (TH1F*)data_both[mode]->createHistogram(("h_data_" + mode).c_str(), Bd_M);
        TH1F * h_fit = (TH1F*)fitShapes[mode]->createHistogram(("h_fit_" + mode).c_str(), Bd_M, RooFit::Binning(nBins * 10));

        // Convert fit components
        TH1F * h_signal = (TH1F*)signal_shape->createHistogram(("h_signal_" + mode).c_str(), Bd_M, RooFit::Binning(nBins * 10));
        TH1F * h_expo = (TH1F*)expo_shapes[mode]->createHistogram(("h_signal_" + mode).c_str(), Bd_M, RooFit::Binning(nBins * 10));
        TH1F * h_low = (TH1F*)low_shapes[mode]->createHistogram(("h_signal_" + mode).c_str(), Bd_M, RooFit::Binning(nBins * 10));

        // Scale histograms
        h_fit->Scale(h_data->Integral() * 10 / h_fit->Integral());
        h_signal->Scale(yields[mode]["n_signal"]->getVal() * 10 / h_signal->Integral());
        h_expo->Scale(yields[mode]["n_expo"]->getVal() * 10 / h_expo->Integral());
        h_low->Scale(yields[mode]["n_low"]->getVal() * 10 / h_low->Integral());

        // Save unblinded histos for Kpi
        if (mode == "Kpi") {
            // Save histograms
            outfile->cd();
            h_data->Write(("data_" + mode).c_str());
            h_fit->Write(("fit_" + mode).c_str());
            h_signal->Write(("signal_" + mode).c_str());
            h_expo->Write(("expo_" + mode).c_str());
            h_low->Write(("low_" + mode).c_str());

            // Save pulls
            RooPlot * frame = Bd_M.frame();
            data_both[mode]->plotOn(frame);
            fitShapes[mode]->plotOn(frame);
            RooHist * pulls = frame->pullHist();
            pulls->Write(("pulls_" + mode).c_str());

        } else {
            // Make Bs histograms
            TH1F * h_Bs = (TH1F*)Bs_shape->createHistogram(("h_Bs_" + mode).c_str(), Bd_M, RooFit::Binning(nBins * 10));
            TH1F * h_Bs_low = (TH1F*)Bs_low_shape->createHistogram(("h_Bs_low_" + mode).c_str(), Bd_M, 
                    RooFit::Binning(nBins * 10));

            // Scale Bs histograms
            h_Bs->Scale(yields[mode]["n_Bs"]->getVal() * 10 / h_Bs->Integral());
            h_Bs_low->Scale(yields[mode]["n_Bs_low"]->getVal() * 10 / h_Bs_low->Integral());

            // Make blind histograms
            const double lsb = 5279.61 - 50;
            const double usb = 5279.61 + 50;
            TH1F * h_data_blind = h_data;
            TH1F * h_fit_blind = h_fit;
            TH1F * h_signal_blind = h_signal;
            TH1F * h_expo_blind = h_expo;
            TH1F * h_low_blind = h_low;
            TH1F * h_Bs_blind = h_Bs;
            TH1F * h_Bs_low_blind = h_Bs_low;

            // Remove data from blind region
            for (int bin = 1; bin < nBins; bin++) {
                double bin_val = h_data->GetBinCenter(bin);
                if (bin_val > lsb && bin_val < usb) h_data_blind->SetBinContent(bin, 0);
            }

            // Remove fits from blind region
            for (int bin = 1; bin < nBins * 10; bin++) {
                double bin_val = h_fit->GetBinCenter(bin);
                if (bin_val > lsb && bin_val < usb) {
                    h_fit_blind->SetBinContent(bin, 0);
                    h_signal_blind->SetBinContent(bin, 0);
                    h_expo_blind->SetBinContent(bin, 0);
                    h_low_blind->SetBinContent(bin, 0);
                    h_Bs_blind->SetBinContent(bin, 0);
                    h_Bs_low_blind->SetBinContent(bin, 0);
                }
            }

            // Save blind histograms to file
            outfile->cd();
            h_data_blind->Write(("data_" + mode).c_str());
            h_fit_blind->Write(("fit_" + mode).c_str());
            h_signal_blind->Write(("signal_" + mode).c_str());
            h_expo_blind->Write(("expo_" + mode).c_str());
            h_low_blind->Write(("low_" + mode).c_str());
            h_Bs_blind->Write(("Bs_" + mode).c_str());
            h_Bs_low_blind->Write(("Bs_low_" + mode).c_str());

            // Save upper and lower pulls
            RooPlot * frame_low = Bd_M.frame();
            RooPlot * frame_high = Bd_M.frame();
            data_both[mode]->plotOn(frame_low, RooFit::Range(5000, lsb));
            data_both[mode]->plotOn(frame_high, RooFit::Range(usb, 5800));
            fitShapes[mode]->plotOn(frame_low, RooFit::Range(5000, lsb));
            fitShapes[mode]->plotOn(frame_high, RooFit::Range(usb, 5800));
            RooHist * pulls_low = frame_low->pullHist();
            RooHist * pulls_high = frame_high->pullHist();
            pulls_low->Write(("pulls_low_" + mode).c_str());
            pulls_high->Write(("pulls_high_" + mode).c_str());

        }
    }
    outfile->Close();

    // ================
    // Plot the results
    // ================
    Plotter * plotter = new Plotter();
    plotter->plotTwoModeFitsCombined("../Histograms/fits_twoBody.root", "twoBody", "");
    delete plotter;

    return 0;
}
