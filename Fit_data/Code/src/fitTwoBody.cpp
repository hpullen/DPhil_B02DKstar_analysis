#include <iostream>
#include <fstream>
#include <vector>

#include "TApplication.h"
#include "TChain.h"
#include "TFile.h"

#include "RooAddPdf.h"
#include "RooArgList.h"
#include "RooArgSet.h"
#include "RooCategory.h"
#include "RooCBShape.h"
#include "RooDataSet.h"
#include "RooExponential.h"
#include "RooFitResult.h"
#include "RooFormulaVar.h"
#include "RooRealVar.h"
#include "RooSimultaneous.h"

#include "RooHORNSdini.h"
#include "RooHILLdini.h"
#include "RooLITTLEHORNSdini.h"

#include "ParameterReader.hpp"
#include "ParameterManager.hpp"
#include "ShapeManager.hpp"

int main(int argc, char * argv[]) {

    // Check for parameters
    if (argc != 3) {
        std::cout << "Usage: ./FitTwoBody <2011:2012:2015:2016> <Sum: Y/N>"
            << std::cout;
        return -1;
    }

    // Make TApplication to display result
    //TApplication * app = new TApplication("app", &argc, argv);

    // Year (colon separated string)
    std::string input_year = argv[1];

    // Sum over charge (Y/N)
    std::string sum = argv[2];

    // Vectors of years and D0 modes
    //std::string years[4] = {"2011", "2012", "2015", "2016"};
    std::vector<std::string> years = {"run1", "run2"};
    std::vector<std::string> modes = {"Kpi", "piK", "KK", "pipi"};

    // Path to data tuples
    std::string path = "/data/lhcb/users/pullen/gangadir/ntuples/reduced_ntuples/With_MVA/Data/";

    // ================
    // Set up variables
    // ================
    // Add double swap mass
    //RooRealVar Bd_M("Bd_ConsD_M", "", 5000, 5800);
    RooRealVar Bd_M("Bd_M", "", 5000, 5800);
    RooRealVar KstarK_ID("KstarK_ID", "", -100000000, 100000000);
    RooRealVar D0_FDS("D0_FDS", "", 2, 100000000);
    RooRealVar D0_M("D0_M", "", 1864.83 - 25, 1864.83 + 25);

    // BDT responses
    RooRealVar BDT_Kpi("BDTG_Kpi_run2", "", 0.5, 1);
    RooRealVar BDT_KK("BDTG_KK_run2", "", 0.5, 1);
    RooRealVar BDT_pipi("BDTG_pipi_run2", "", 0.5, 1);

    // PID variables
    RooRealVar KstarK_PIDK("KstarK_PIDK", "", 3, 100000000);
    RooRealVar KstarPi_PIDK("KstarPi_PIDK", "", -100000000, -1);
    RooRealVar D0K_PIDK("D0K_PIDK", "", 1, 100000000);
    RooRealVar D0Pi_PIDK("D0Pi_PIDK", "", -100000000, -1);
    RooRealVar D0Kplus_PIDK("D0Kplus_PIDK", "", 1, 100000000);
    RooRealVar D0Kminus_PIDK("D0Kminus_PIDK", "", 1, 100000000);
    RooRealVar D0PiPlus_PIDK("D0PiPlus_PIDK", "", -100000000, -1);
    RooRealVar D0PiMinus_PIDK("D0PiMinus_PIDK", "", -100000000, -1);

    // Make list of shared args
    std::map<std::string, RooArgList *> argMap;
    for (auto mode : modes) {
        argMap[mode] = new RooArgList();
        argMap[mode]->add(Bd_M);
        argMap[mode]->add(KstarK_ID);
        argMap[mode]->add(D0_FDS);
        argMap[mode]->add(D0_M);
        argMap[mode]->add(KstarK_PIDK);
        argMap[mode]->add(KstarPi_PIDK);
    }

    // Kpi/piK args
    argMap["Kpi"]->add(BDT_Kpi);
    argMap["Kpi"]->add(D0K_PIDK);
    argMap["Kpi"]->add(D0Pi_PIDK);
    argMap["piK"]->add(BDT_Kpi);
    argMap["piK"]->add(D0K_PIDK);
    argMap["piK"]->add(D0Pi_PIDK);

    // KK args
    argMap["KK"]->add(BDT_KK);
    argMap["KK"]->add(D0Kplus_PIDK);
    argMap["KK"]->add(D0Kminus_PIDK);

    // pipi args
    argMap["pipi"]->add(BDT_pipi);
    argMap["pipi"]->add(D0PiPlus_PIDK);
    argMap["pipi"]->add(D0PiMinus_PIDK);

    // ======================================
    // Loop over modes and years and add data
    // ======================================
    std::map<std::string, TChain *> treeMap;
    std::map<std::string, RooDataSet *> dataMap;
    std::map<std::string, RooDataSet *> dataMapSplit;
    //for (auto mode : modes) {

        //// Make TChain to hold data
        //treeMap[mode] = new TChain("DecayTree");

        //// Loop over years
        //for (auto year : years) {
            //if (input_year.find(year) != std::string::npos) {
                //// Change this later to add up and down
                //std::string bdt_mode = mode;
                //if (mode == "piK") bdt_mode = "Kpi";
                //treeMap[mode]->Add((path + mode + "_" + year + "_BDTG_" + 
                            //bdt_mode + "_run2.root").c_str());
            //}
        //}

        //// Turn on relevant branches only
        //treeMap[mode]->SetBranchStatus("*", 0);
        ////treeMap[mode]->SetBranchStatus("Bd_ConsD_M", 1);
        //treeMap[mode]->SetBranchStatus("Bd_M", 1);
        //treeMap[mode]->SetBranchStatus("KstarK_ID");
        //treeMap[mode]->SetBranchStatus("D0_FDS", 1);
        //treeMap[mode]->SetBranchStatus("D0_M", 1);
        //treeMap[mode]->SetBranchStatus("KstarK_PIDK", 1);
        //treeMap[mode]->SetBranchStatus("KstarK_PIDK");

        //// Mode-specific branches
        //if (mode == "Kpi" || mode == "piK") {
            //treeMap[mode]->SetBranchStatus("BDTG_Kpi_run2", 1);
            //treeMap[mode]->SetBranchStatus("D0K_PIDK", 1);
            //treeMap[mode]->SetBranchStatus("D0Pi_PIDK", 1);
        //} else if (mode == "KK") {
            //treeMap[mode]->SetBranchStatus("BDTG_KK_run2", 1);
            //treeMap[mode]->SetBranchStatus("D0Kplus_PIDK", 1);
            //treeMap[mode]->SetBranchStatus("D0Kminus_PIDK", 1);
        //} else if (mode == "pipi") {
            //treeMap[mode]->SetBranchStatus("BDTG_pipi_run2", 1);
            //treeMap[mode]->SetBranchStatus("D0PiPlus_PIDK", 1);
            //treeMap[mode]->SetBranchStatus("D0PiMinus_PIDK", 1);
        //}

        //// Create RooDataSet for the sample
        //std::cout << "Making RooDataSet for " << mode << std::endl;
        //dataMap[mode] = new RooDataSet(("data_" + mode).c_str(), "", treeMap[mode],
                //RooArgSet(*argMap[mode]));
        //std::cout << "Created RooDataSet for mode " << mode << std::endl;
        //dataMap[mode]->Print();

        //// Split by flavour if requested
        //if (sum == "N") {
            //dataMapSplit[mode + "_plus"] = (RooDataSet*)(dataMap[mode]->reduce("KstarK_ID > 0");
            //dataMapSplit[mode + "_minus"] = (RooDataSet*)(dataMap[mode]->reduce("KstarK_ID < 0");
        //}

    //}

    // =========================
    // Read in MC fit parameters
    // =========================
    ParameterReader pr;
    std::string MC_path = "/home/pullen/analysis/B02DKstar/Fit_monte_carlo/Results/";
    pr.readParams("Kpi", MC_path + "signal_Kpi.param");
    pr.readParams("Bs", MC_path + "signal_Bs.param");
    pr.readParams("low", MC_path + "lowMass.param");

    // Parameter manager to hold fit parameters
    ParameterManager pm("parameters");
    pm.addParam("Bd_M", &Bd_M, ParamType::real);

    // ==============================
    // Make signal (B0 and Bs) shapes
    // ==============================
    // Signal (crystal ball) fit parameters
    pm.addParam("signal_mean", pr["Kpi_mean"], pr["Kpi_mean"] - 10,
            pr["Kpi_mean"] + 10);
    pm.addParam("signal_sigma_L", pr["Kpi_sigma_L"]);
    pm.addParam("signal_sigma_R", pr["Kpi_sigma_R"]);
    pm.addParam("signal_alpha_L", pr["Kpi_alpha_L"]);
    pm.addParam("signal_alpha_R", pr["Kpi_alpha_R"]);
    pm.addParam("signal_n_L", pr["Kpi_n_L"]);
    pm.addParam("signal_n_R", pr["Kpi_n_R"]);
    pm.addParam("signal_frac", pr["Kpi_frac"]);

    // Bs (crystal ball) shape parameters
    pm.addParam("delta_M", 87.26, 80, 90); 
    pm.addFormulaVar("Bs_mean", "@0 + @1", {{0, "signal_mean"}, {1, "delta_M"}}, &pm);
    pm.addParam("Bs_sigma_L", pr["Kpi_sigma_L"]);
    pm.addParam("Bs_sigma_R", pr["Kpi_sigma_R"]);
    pm.addParam("Bs_alpha_L", pr["Kpi_alpha_L"]);
    pm.addParam("Bs_alpha_R", pr["Kpi_alpha_R"]);
    pm.addParam("Bs_n_L", pr["Kpi_n_L"]);
    pm.addParam("Bs_n_R", pr["Kpi_n_R"]);
    pm.addParam("Bs_frac", pr["Kpi_frac"]);

    // Shape manager to hold PDFs
    ShapeManager sm("shapes");

    // Make signal crystal ball PDFs
    sm.addCrystalBall("signal_CB_L", &pm, "Bd_M", "signal_mean", "signal_sigma_L",
            "signal_alpha_L", "signal_n_L");
    sm.addCrystalBall("signal_CB_R", &pm, "Bd_M", "signal_mean", "signal_sigma_R",
            "signal_alpha_R", "signal_n_R");
    sm.addCrystalBall("Bs_CB_L", &pm, "Bd_M", "Bs_mean", "Bs_sigma_L",
            "Bs_alpha_L", "Bs_n_L");
    sm.addCrystalBall("Bs_CB_R", &pm, "Bd_M", "Bs_mean", "Bs_sigma_R",
            "Bs_alpha_R", "Bs_n_R");

    // Add the PDFs to make double crystal balls
    sm.addAddPdf("signal", &pm, &sm, {{"signal_CB_L", "signal_frac"},
            {"signal_CB_R", ""}});
    sm.addAddPdf("Bs", &pm, &sm, {{"Bs_CB_L", "Bs_frac"},
            {"Bs_CB_R", ""}});

    // =========================
    // Make combinatorial shapes
    // =========================
    // Combinatorial slopes
    pm.addParam("slope_Kpi", -0.1, -0.5, 0);
    pm.addParam("slope_KK", -0.1, -0.5, 0);
    pm.addParam("slope_pipi", -0.1, -0.5, 0);

    // Make combinatorial shapes
    sm.addExponential("expo_Kpi", &pm, "Bd_M", "slope_Kpi");
    sm.addExponential("expo_KK", &pm, "Bd_M", "slope_KK");
    sm.addExponential("expo_pipi", &pm, "Bd_M", "slope_pipi");

    // ====================
    // Make low mass shapes
    // ====================
    // Kinematic endpoints
    pm.addParam("a_gamma", pr["low_a_gamma"]);
    pm.addParam("b_gamma", pr["low_b_gamma"]);
    pm.addParam("a_pi", pr["low_a_pi"]);
    pm.addParam("b_pi", pr["low_b_pi"]);

    // Widths
    pm.addParam("sigma_gamma_010", pr["low_sigma_gamma_010"]);
    pm.addParam("sigma_gamma_101", pr["low_sigma_gamma_101"]);
    pm.addParam("sigma_pi_010", pr["low_sigma_pi_010"]);
    pm.addParam("sigma_pi_101", pr["low_sigma_pi_101"]);

    // Csi 
    pm.addParam("csi_gamma_010", pr["low_csi_gamma_010"]);
    pm.addParam("csi_gamma_101", pr["low_csi_gamma_101"]);
    pm.addParam("csi_pi_010", pr["low_csi_pi_010"],
            pr["low_csi_pi_010"] - 0.5, pr["low_csi_pi_010"] + 0.2);
    pm.addParam("csi_pi_101", pr["low_csi_pi_101"]);

    // Bs widths
    pm.addParam("Bs_sigma_gamma_010", pr["low_Bs_sigma_gamma_010"]);
    pm.addParam("Bs_sigma_gamma_101", pr["low_Bs_sigma_gamma_101"]);
    pm.addParam("Bs_sigma_pi_010", pr["low_Bs_sigma_pi_010"]);
    pm.addParam("Bs_sigma_pi_101", pr["low_Bs_sigma_pi_101"]);

    // Bs csi
    pm.addParam("Bs_csi_gamma_010", pr["low_Bs_csi_gamma_010"]);
    pm.addParam("Bs_csi_gamma_101", pr["low_Bs_csi_gamma_101"]);
    pm.addParam("Bs_csi_pi_010", pr["low_Bs_csi_pi_010"],
            pr["low_Bs_csi_pi_010"] - 0.5, pr["low_Bs_csi_pi_010"] + 0.2);
    pm.addParam("Bs_csi_pi_101", pr["low_Bs_csi_pi_101"]);

    // Fraction and ratio
    pm.addParam("low_frac", pr["low_frac"]);
    pm.addParam("low_ratio", pr["low_ratio"]);

    // Global shift
    pm.addParam("shift", 0, -10, 10);
    pm.addParam("shiftg", (double)0.0);

    // Shifted endpoints for Bs
    pm.addFormulaVar("Bs_a_gamma", "@0 + @1", {{0, "a_gamma"}, {1, "delta_M"}}, &pm);
    pm.addFormulaVar("Bs_b_gamma", "@0 + @1", {{0, "b_gamma"}, {1, "delta_M"}}, &pm);
    pm.addFormulaVar("Bs_a_pi", "@0 + @1", {{0, "a_pi"}, {1, "delta_M"}}, &pm);
    pm.addFormulaVar("Bs_b_pi", "@0 + @1", {{0, "b_pi"}, {1, "delta_M"}}, &pm);

    // Make shape for each component
    // B0 low mass shapes
    sm.addHill("gamma_010", &pm, "Bd_M", "a_gamma", "b_gamma", "csi_gamma_010",
            "shift", "sigma_gamma_010", "low_ratio", "low_frac");
    sm.addLittleHorns("gamma_101", &pm, "Bd_M", "a_gamma", "b_gamma", "csi_gamma_101",
            "shift", "sigma_gamma_101", "low_ratio", "low_frac", "shiftg");
    sm.addHorns("pi_010", &pm, "Bd_M", "a_pi", "b_pi", "csi_pi_010", "shift", 
            "sigma_pi_010", "low_ratio", "low_frac");
    sm.addHill("pi_101", &pm, "Bd_M", "a_pi", "b_pi", "csi_pi_101", "shift", 
            "sigma_pi_101", "low_ratio", "low_frac");

    // Bs low mass shapes
    sm.addHill("Bs_gamma_010", &pm, "Bd_M", "Bs_a_gamma", "Bs_b_gamma", 
            "Bs_csi_gamma_010", "shift", "Bs_sigma_gamma_010", "low_ratio", 
            "low_frac");
    sm.addLittleHorns("Bs_gamma_101", &pm, "Bd_M", "Bs_a_gamma", "Bs_b_gamma", 
            "Bs_csi_gamma_101", "shift", "Bs_sigma_gamma_101", "low_ratio", 
            "low_frac", "shiftg");
    sm.addHorns("Bs_pi_010", &pm, "Bd_M", "Bs_a_pi", "Bs_b_pi", "Bs_csi_pi_010", 
            "shift", "Bs_sigma_pi_010", "low_ratio", "low_frac");
    sm.addHill("Bs_pi_101", &pm, "Bd_M", "Bs_a_pi", "Bs_b_pi", "Bs_csi_pi_101", 
            "shift", "Bs_sigma_pi_101", "low_ratio", "low_frac");

    // Calculate fractions of pi and gamma
    const double branching_gamma = 0.353;
    const double branching_pi = 0.647;
    const double acc_gamma = 17.48;
    const double acc_pi = 15.53;
    double G_010 = branching_gamma * acc_gamma * 0.009227;
    double G_101 = branching_gamma * acc_gamma * 0.004613; 
    double P_010 = branching_pi * acc_pi * 0.01168;
    double P_101 = branching_pi * acc_pi * 0.005712;
    
    // Add to parameters
    pm.addParam("coeff_gamma_010", G_010 / (P_010 + G_010));
    pm.addParam("coeff_gamma_101", G_101 / (P_101 + G_101));
    pm.addParam("coeff_pi_010", P_010 / (P_010 + G_010));
    pm.addParam("coeff_pi_101", P_101 / (P_101 + G_101));

    // Combine shapes into helicity components
    sm.addAddPdf("low_010", &pm, &sm, {{"gamma_010", "coeff_gamma_010"}, {"pi_010", "coeff_pi_010"}});
    sm.addAddPdf("low_101", &pm, &sm, {{"gamma_101", "coeff_gamma_101"}, {"pi_101", "coeff_pi_101"}});
    sm.addAddPdf("Bs_low_010", &pm, &sm, {{"Bs_gamma_010", "coeff_gamma_010"}, {"Bs_pi_010", "coeff_pi_010"}});
    sm.addAddPdf("Bs_low_101", &pm, &sm, {{"Bs_gamma_101", "coeff_gamma_101"}, {"Bs_pi_101", "coeff_pi_101"}});

    // ==================
    // Set up asymmetries
    // ==================
    // Parameter manager to hold physics observables
    ParameterManager obs("observables");

    // Signal asymmetries
    obs.addParam("A_Kpi", 0, -1, 1);
    obs.addFormulaVar("a_Kpi", "(1 + @0) / (1 - @0)", "A_Kpi", &obs);
    obs.addBlindParam("A_piK", 0, -1, 1, "blind_A_piK", 0.05);
    obs.addFormulaVar("a_piK", "(1 + @0) / (1 - @0)", "A_piK", &obs);
    obs.addBlindParam("A_KK", 0, -1, 1, "blind_A_KK", 0.05);
    obs.addFormulaVar("a_KK", "(1 + @0) / (1 - @0)", "A_KK", &obs);
    obs.addBlindParam("A_pipi", 0, -1, 1, "blind_A_pipi", 0.05);
    obs.addFormulaVar("a_pipi", "(1 + @0) / (1 - @0)", "A_pipi", &obs);

    // Bs asymmetries
    obs.addParam("A_piK_Bs", 0, -1, 1);
    obs.addFormulaVar("a_piK_Bs", "(1 + @0) / (1 - @0)", "A_piK_Bs", &obs);
    obs.addParam("A_KK_Bs", 0, -1, 1);
    obs.addFormulaVar("a_KK_Bs", "(1 + @0) / (1 - @0)", "A_KK_Bs", &obs);
    obs.addParam("A_pipi_Bs", 0, -1, 1);
    obs.addFormulaVar("a_pipi_Bs", "(1 + @0) / (1 - @0)", "A_pipi_Bs", &obs);

    // Low mass asymmetries
    obs.addParam("A_Kpi_low", 0, -1, 1);
    obs.addFormulaVar("a_Kpi_low", "(1 + @0) / (1 - @0)", "A_Kpi_low", &obs);
    obs.addParam("A_piK_low", 0, -1, 1);
    obs.addFormulaVar("a_piK_low", "(1 + @0) / (1 - @0)", "A_piK_low", &obs);
    obs.addParam("A_KK_low", 0, -1, 1);
    obs.addFormulaVar("a_KK_low", "(1 + @0) / (1 - @0)", "A_KK_low", &obs);
    obs.addParam("A_pipi_low", 0, -1, 1);
    obs.addFormulaVar("a_pipi_low", "(1 + @0) / (1 - @0)", "A_pipi_low", &obs);

    // Low mass Bs asymmetries (fixed to zero)
    obs.addParam("A_piK_Bs_low", (double)0);
    obs.addFormulaVar("a_piK_Bs_low", "(1 + @0) / (1 - @0)", "A_piK_Bs_low", &obs);
    obs.addParam("A_KK_Bs_low", (double)0);
    obs.addFormulaVar("a_KK_Bs_low", "(1 + @0) / (1 - @0)", "A_KK_Bs_low", &obs);
    obs.addParam("A_pipi_Bs_low", (double)0);
    obs.addFormulaVar("a_pipi_Bs_low", "(1 + @0) / (1 - @0)", "A_pipi_Bs_low", &obs);

    // ===========================
    // Set up ratios between modes
    // ===========================
    // Signal ratios (blind)
    obs.addBlindParam("R_piK_vs_Kpi", 0.06, 0, 1, "blind_R_piK_vs_Kpi", 0.01);
    obs.addBlindParam("R_KK_vs_Kpi", 0.1, 0, 1, "blind_R_KK_vs_Kpi", 0.05);
    obs.addBlindParam("R_pipi_vs_Kpi", 0.03, 0, 1, "blind_R_pipi_vs_Kpi", 0.01);

    // Bs ratios
    obs.addParam("R_KK_vs_piK_Bs", 0.1, 0, 1);
    obs.addParam("R_pipi_vs_piK_Bs", 0.1, 0, 1);

    // Low mass ratios
    obs.addParam("R_piK_vs_Kpi_low", 0.06, 0, 1);
    obs.addParam("R_KK_vs_Kpi_low", 0.1, 0, 1);
    obs.addParam("R_pipi_vs_Kpi_low", 0.03, 0, 1);

    // Low mass Bs ratios
    obs.addParam("R_KK_vs_piK_Bs_low", 0.1, 0, 1);
    obs.addParam("R_pipi_vs_piK_Bs_low", 0.1, 0, 1);

    // Helicity fractions
    obs.addParam("a_010_Kpi", 0.5, 0, 10);
    obs.addParam("a_010_piK", 0.5, 0, 10);
    obs.addParam("a_010_piK_plus", 0.5, 0, 10);
    obs.addParam("a_010_piK_minus", 0.5, 0, 10);
    obs.addParam("a_010_GLW", 0.5, 0, 10);
    obs.addParam("a_010_GLW_plus", 0.5, 0, 10);
    obs.addParam("a_010_GLW_minus", 0.5, 0, 10);
    obs.addParam("a_010_Bs", 0.5, 0, 10);

    // =============
    // Set up yields
    // =============
    // Parameter managers to hold yields
    std::map<std::string, ParameterManager *> yields;
    yields["Kpi"] = new ParameterManager("yields_Kpi");
    yields["piK"] = new ParameterManager("yields_piK");
    yields["KK"] = new ParameterManager("yields_KK");
    yields["pipi"] = new ParameterManager("yields_pipi");

    // Total signal yields for each mode
    yields["Kpi"]->addParam("signal", 1000, 0, 20000); 
    yields["piK"]->addFormulaVar("signal", "@0 * @1", yields["Kpi"], "signal", &obs, "R_piK_vs_Kpi");
    yields["KK"]->addFormulaVar("signal", "@0 * @1", yields["Kpi"], "signal", &obs, "R_KK_vs_Kpi");
    yields["pipi"]->addFormulaVar("signal", "@0 * @1", yields["Kpi"], "signal", &obs, "R_pipi_vs_Kpi");

    // Signal yields split by flavour (plus = B0, minus = B0 bar)
    yields["Kpi"]->addFormulaVar("signal_plus", "@0 / (1 + @1)", yields["Kpi"], "signal", &obs, "a_Kpi");
    yields["Kpi"]->addFormulaVar("signal_minus", "@0 / (1 + 1/@1)", yields["Kpi"], "signal", &obs, "a_Kpi");
    yields["piK"]->addFormulaVar("signal_plus", "@0 / (1 + @1)", yields["piK"], "signal", &obs, "a_piK");
    yields["piK"]->addFormulaVar("signal_minus", "@0 / (1 + 1/@1)", yields["piK"], "signal", &obs, "a_piK");
    yields["KK"]->addFormulaVar("signal_plus", "@0 / (1 + @1)", yields["KK"], "signal", &obs, "a_KK");
    yields["KK"]->addFormulaVar("signal_minus", "@0 / (1 + 1/@1)", yields["KK"], "signal", &obs, "a_KK");
    yields["pipi"]->addFormulaVar("signal_plus", "@0 / (1 + @1)", yields["pipi"], "signal", &obs, "a_pipi");
    yields["pipi"]->addFormulaVar("signal_minus", "@0 / (1 + 1/@1)", yields["pipi"], "signal", &obs, "a_pipi");

    // Total Bs yields
    yields["piK"]->addParam("Bs", 1000, 0, 20000);
    yields["KK"]->addFormulaVar("Bs", "@0 * @1", yields["piK"], "Bs", &obs, "R_KK_vs_piK_Bs");
    yields["pipi"]->addFormulaVar("Bs", "@0 * @1", yields["piK"], "Bs", &obs, "R_pipi_vs_piK_Bs");

    // Bs yields split by flavour (to do: check order of asymmetries)
    yields["piK"]->addFormulaVar("Bs_plus", "@0 / (1 + 1/@1)", yields["piK"], "Bs", &obs, "a_piK_Bs");
    yields["piK"]->addFormulaVar("Bs_minus", "@0 / (1 + @1)", yields["piK"], "Bs", &obs, "a_piK_Bs");
    yields["KK"]->addFormulaVar("Bs_plus", "@0 / (1 + 1/@1)", yields["KK"], "Bs", &obs, "a_KK_Bs");
    yields["KK"]->addFormulaVar("Bs_minus", "@0 / (1 + @1)", yields["KK"], "Bs", &obs, "a_KK_Bs");
    yields["pipi"]->addFormulaVar("Bs_plus", "@0 / (1 + 1/@1)", yields["pipi"], "Bs", &obs, "a_pipi_Bs");
    yields["pipi"]->addFormulaVar("Bs_minus", "@0 / (1 + @1)", yields["pipi"], "Bs", &obs, "a_pipi_Bs");

    // Total low mass yields
    yields["Kpi"]->addParam("low", 1000, 0, 20000);
    yields["piK"]->addFormulaVar("low", "@0 * @1", yields["Kpi"], "low", &obs, "R_piK_vs_Kpi_low");
    yields["KK"]->addFormulaVar("low", "@0 * @1", yields["Kpi"], "low", &obs, "R_KK_vs_Kpi_low");
    yields["pipi"]->addFormulaVar("low", "@0 * @1", yields["Kpi"], "low", &obs, "R_pipi_vs_Kpi_low");

    // Low mass yields split by helicity
    yields["Kpi"]->addFormulaVar("low_010", "@0 * @1", yields["Kpi"], "low", &obs, "a_010_Kpi");
    yields["Kpi"]->addFormulaVar("low_101", "@0 * (1 - @1)", yields["Kpi"], "low", &obs, "a_010_Kpi");
    yields["piK"]->addFormulaVar("low_010", "@0 * @1", yields["piK"], "low", &obs, "a_010_piK");
    yields["piK"]->addFormulaVar("low_101", "@0 * (1 - @1)", yields["piK"], "low", &obs, "a_010_piK");
    yields["KK"]->addFormulaVar("low_010", "@0 * @1", yields["KK"], "low", &obs, "a_010_GLW");
    yields["KK"]->addFormulaVar("low_101", "@0 * (1 - @1)", yields["KK"], "low", &obs, "a_010_GLW");
    yields["pipi"]->addFormulaVar("low_010", "@0 * @1", yields["pipi"], "low", &obs, "a_010_GLW");
    yields["pipi"]->addFormulaVar("low_101", "@0 * (1 - @1)", yields["pipi"], "low", &obs, "a_010_GLW");

    // Low mass yields split by flavour
    yields["Kpi"]->addFormulaVar("low_plus", "@0 / (1 + @1)", yields["Kpi"], "low", &obs, "a_Kpi");
    yields["Kpi"]->addFormulaVar("low_minus", "@0 / (1 + 1/@1)", yields["Kpi"], "low", &obs, "a_Kpi");
    yields["piK"]->addFormulaVar("low_plus", "@0 / (1 + @1)", yields["piK"], "low", &obs, "a_piK");
    yields["piK"]->addFormulaVar("low_minus", "@0 / (1 + 1/@1)", yields["piK"], "low", &obs, "a_piK");
    yields["KK"]->addFormulaVar("low_plus", "@0 / (1 + @1)", yields["KK"], "low", &obs, "a_KK");
    yields["KK"]->addFormulaVar("low_minus", "@0 / (1 + 1/@1)", yields["KK"], "low", &obs, "a_KK");
    yields["pipi"]->addFormulaVar("low_plus", "@0 / (1 + @1)", yields["pipi"], "low", &obs, "a_pipi");
    yields["pipi"]->addFormulaVar("low_minus", "@0 / (1 + 1/@1)", yields["pipi"], "low", &obs, "a_pipi");

    // Low mass yields split by flavour and helicity
    yields["Kpi"]->addFormulaVar("low_010_plus", "@0 * @1", yields["Kpi"], "low_plus", &obs, "a_010_Kpi");
    yields["Kpi"]->addFormulaVar("low_101_plus", "@0 * (1 - @1)", yields["Kpi"], "low_plus", &obs, "a_010_Kpi");
    yields["Kpi"]->addFormulaVar("low_010_minus", "@0 * @1", yields["Kpi"], "low_minus", &obs, "a_010_Kpi");
    yields["Kpi"]->addFormulaVar("low_101_minus", "@0 * (1 - @1)", yields["Kpi"], "low_minus", &obs, "a_010_Kpi");
    yields["piK"]->addFormulaVar("low_010_plus", "@0 * @1", yields["piK"], "low_plus", &obs, "a_010_piK_plus");
    yields["piK"]->addFormulaVar("low_101_plus", "@0 * (1 - @1)", yields["piK"], "low_plus", &obs, "a_010_piK");
    yields["piK"]->addFormulaVar("low_010_minus", "@0 * @1", yields["piK"], "low_minus", &obs, "a_010_piK_minus");
    yields["piK"]->addFormulaVar("low_101_minus", "@0 * (1 - @1)", yields["piK"], "low_minus", &obs, "a_010_piK");
    yields["KK"]->addFormulaVar("low_010_plus", "@0 * @1", yields["KK"], "low_plus", &obs, "a_010_GLW_plus");
    yields["KK"]->addFormulaVar("low_101_plus", "@0 * (1 - @1)", yields["KK"], "low_plus", &obs, "a_010_GLW");
    yields["KK"]->addFormulaVar("low_010_minus", "@0 * @1", yields["KK"], "low_minus", &obs, "a_010_GLW_minus");
    yields["KK"]->addFormulaVar("low_101_minus", "@0 * (1 - @1)", yields["KK"], "low_minus", &obs, "a_010_GLW");
    yields["pipi"]->addFormulaVar("low_010_plus", "@0 * @1", yields["pipi"], "low_plus", &obs, "a_010_GLW_plus");
    yields["pipi"]->addFormulaVar("low_101_plus", "@0 * (1 - @1)", yields["pipi"], "low_plus", &obs, "a_010_GLW");
    yields["pipi"]->addFormulaVar("low_010_minus", "@0 * @1", yields["pipi"], "low_minus", &obs, "a_010_GLW_minus");
    yields["pipi"]->addFormulaVar("low_101_minus", "@0 * (1 - @1)", yields["pipi"], "low_minus", &obs, "a_010_GLW");

    // Total Bs low mass yields
    yields["piK"]->addParam("low_Bs", 1000, 0, 20000);
    yields["KK"]->addFormulaVar("low_Bs", "@0 * @1", yields["piK"], "low_Bs", &obs, "R_KK_vs_piK_Bs_low");
    yields["pipi"]->addFormulaVar("low_Bs", "@0 * @1", yields["piK"], "low_Bs", &obs, "R_pipi_vs_piK_Bs_low");

    // Bs low mass yields split by helicity
    yields["piK"]->addFormulaVar("low_010_Bs", "@0 * @1", yields["piK"], "low_Bs", &obs, "a_010_Bs");
    yields["piK"]->addFormulaVar("low_101_Bs", "@0 * (1 - @1)", yields["piK"], "low_Bs", &obs, "a_010_Bs");
    yields["KK"]->addFormulaVar("low_010_Bs", "@0 * @1", yields["KK"], "low_Bs", &obs, "a_010_Bs");
    yields["KK"]->addFormulaVar("low_101_Bs", "@0 * (1 - @1)", yields["KK"], "low_Bs", &obs, "a_010_Bs");
    yields["pipi"]->addFormulaVar("low_010_Bs", "@0 * @1", yields["pipi"], "low_Bs", &obs, "a_010_Bs");
    yields["pipi"]->addFormulaVar("low_101_Bs", "@0 * (1 - @1)", yields["pipi"], "low_Bs", &obs, "a_010_Bs");

    // Bs low mass yields split by helicity and flavour
    yields["piK"]->addFormulaVar("low_010_Bs_plus", "@0 / (1 + 1/@1)", yields["piK"], "low_010_Bs", &obs, "a_piK_Bs_low");
    yields["piK"]->addFormulaVar("low_101_Bs_plus", "@0 / (1 + 1/@1)", yields["piK"], "low_101_Bs", &obs, "a_piK_Bs_low");
    yields["piK"]->addFormulaVar("low_010_Bs_minus", "@0 / (1 + @1)", yields["piK"], "low_010_Bs", &obs, "a_piK_Bs_low");
    yields["piK"]->addFormulaVar("low_101_Bs_minus", "@0 / (1 + @1)", yields["piK"], "low_101_Bs", &obs, "a_piK_Bs_low");
    yields["KK"]->addFormulaVar("low_010_Bs_plus", "@0 / (1 + 1/@1)", yields["KK"], "low_010_Bs", &obs, "a_KK_Bs_low");
    yields["KK"]->addFormulaVar("low_101_Bs_plus", "@0 / (1 + 1/@1)", yields["KK"], "low_101_Bs", &obs, "a_KK_Bs_low");
    yields["KK"]->addFormulaVar("low_010_Bs_minus", "@0 / (1 + @1)", yields["KK"], "low_010_Bs", &obs, "a_KK_Bs_low");
    yields["KK"]->addFormulaVar("low_101_Bs_minus", "@0 / (1 + @1)", yields["KK"], "low_101_Bs", &obs, "a_KK_Bs_low");
    yields["pipi"]->addFormulaVar("low_010_Bs_plus", "@0 / (1 + 1/@1)", yields["pipi"], "low_010_Bs", &obs, "a_pipi_Bs_low");
    yields["pipi"]->addFormulaVar("low_101_Bs_plus", "@0 / (1 + 1/@1)", yields["pipi"], "low_101_Bs", &obs, "a_pipi_Bs_low");
    yields["pipi"]->addFormulaVar("low_010_Bs_minus", "@0 / (1 + @1)", yields["pipi"], "low_010_Bs", &obs, "a_pipi_Bs_low");
    yields["pipi"]->addFormulaVar("low_101_Bs_minus", "@0 / (1 + @1)", yields["pipi"], "low_101_Bs", &obs, "a_pipi_Bs_low");

    // Total combinatorial yields
    yields["Kpi"]->addParam("expo", 1000, 0, 10000);
    yields["piK"]->addParam("expo", 1000, 0, 10000);
    yields["KK"]->addParam("expo", 1000, 0, 10000);
    yields["pipi"]->addParam("expo", 1000, 0, 10000);

    // Combinatorial split by flavour (equal split)
    yields["Kpi"]->addFormulaVar("expo_plus", "@0 / 2", "expo", yields["Kpi"]);
    yields["Kpi"]->addFormulaVar("expo_minus", "@0 / 2", "expo", yields["Kpi"]);
    yields["piK"]->addFormulaVar("expo_plus", "@0 / 2", "expo", yields["piK"]);
    yields["piK"]->addFormulaVar("expo_minus", "@0 / 2", "expo", yields["piK"]);
    yields["KK"]->addFormulaVar("expo_plus", "@0 / 2", "expo", yields["KK"]);
    yields["KK"]->addFormulaVar("expo_minus", "@0 / 2", "expo", yields["KK"]);
    yields["pipi"]->addFormulaVar("expo_plus", "@0 / 2", "expo", yields["pipi"]);
    yields["pipi"]->addFormulaVar("expo_minus", "@0 / 2", "expo", yields["pipi"]);

    // =================
    // Create total PDFs
    // =================
    // Shape manager to hold total PDFs
    ShapeManager fits("fits");

    // Loop through modes and fill maps
    for (auto mode : modes) {


        // Map to hold names of shapes and corresponding yields
        std::map<std::string, std::string> yields_both;
        std::map<std::string, std::string> yields_plus;
        std::map<std::string, std::string> yields_minus;

        // Fill map of shapes
        // Signal shape and yield
        yields_both.emplace("signal", "signal");
        yields_plus.emplace("signal", "signal_plus");
        yields_minus.emplace("signal", "signal");

        // Exponential shape and yield
        std::string expo_name = "expo_" + mode;
        if (mode == "piK") expo_name = "expo_Kpi";
        yields_both.emplace(expo_name, "expo");
        yields_plus.emplace(expo_name, "expo_plus");
        yields_minus.emplace(expo_name, "expo_minus");

        // Low mass helicity components
        yields_both.emplace("low_010", "low_010");
        yields_plus.emplace("low_010", "low_010_plus");
        yields_minus.emplace("low_010", "low_010_minus");
        yields_both.emplace("low_101", "low_101");
        yields_plus.emplace("low_101", "low_101_plus");
        yields_minus.emplace("low_101", "low_101_minus");

        // Bs shapes for all modes except Kpi
        if (mode != "Kpi") {
            // Bs signal
            yields_both.emplace("Bs", "Bs");
            yields_plus.emplace("Bs", "Bs_plus");
            yields_minus.emplace("Bs", "Bs_minus");

            // Bs low mass
            yields_both.emplace("Bs_low_010", "low_010_Bs");
            yields_plus.emplace("Bs_low_010", "low_010_Bs_plus");
            yields_minus.emplace("Bs_low_010", "low_010_Bs_minus");
            yields_both.emplace("Bs_low_101", "low_101_Bs");
            yields_plus.emplace("Bs_low_101", "low_101_Bs_plus");
            yields_minus.emplace("Bs_low_101", "low_101_Bs_minus");
        }

        // Make total PDFs
        fits.addAddPdf(mode + "_both", yields[mode], &sm, yields_both);
        fits.addAddPdf(mode + "_plus", yields[mode], &sm, yields_plus);
        fits.addAddPdf(mode + "_minus", yields[mode], &sm, yields_minus);
    }


    // =====================
    // Make simultaneous fit
    // =====================
    // Fit categories
    RooCategory category("category", "");
    std::map<std::string, RooDataSet *> data_to_use;
    if (sum == "N") {
        for (auto mode : modes) {
            category.defineType((mode + "_plus").c_str());
            category.defineType((mode + "_minus").c_str());
        }
        data_to_use = dataMapSplit;
    } else {
        for (auto mode : modes) {
            category.defineType(mode.c_str());
        }
        data_to_use = dataMap;
    }

    // Make combined dataset and simultaneous fit
    //RooDataSet combData("combData", "", Bd_M, RooFit::Index(category), RooFit::Import(data_to_use));

    // Make simultaneous PDF
    //RooSimultaneous * simPdf = new RooSimultaneous("simPdf", "", category);
    //if (sum == "N") {
        //for (auto mode : modes) {
            //simPdf->addPdf(*(fits[mode + "_plus"]), (mode + "_plus").c_str());
            //simPdf->addPdf(*(fits[mode + "_minus"]), (mode + "_minus").c_str());
        //}
    //} else {
        //for (auto mode : modes) {
            //simPdf->addPdf(*(fits[mode]), (mode + "_both").c_str());
        //}
    //}

    //// Perform fit
    ////RooFitResult * r = simPdf->fitTo(combData, RooFit::Save(), RooFit::NumCPU(8, 2), 
            ////RooFit::Extended(true), RooFit::Optimize(false), RooFit::Offset(true),
            ////RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));





    return 0;

}

