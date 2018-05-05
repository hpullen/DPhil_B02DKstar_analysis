#include <map>
#include <string>

#include "TIterator.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"

#include "RooStats/SPlot.h"
#include "RooAbsReal.h"
#include "RooAddPdf.h"
#include "RooCBShape.h"
#include "RooDataSet.h"
#include "RooExponential.h"
#include "RooFitResult.h"
#include "RooFormulaVar.h"
#include "RooGaussian.h"
#include "RooHILLdini.h"
#include "RooHORNSdini.h"
#include "RooLITTLEHORNSdini.h"
#include "RooPlot.h"
#include "RooRealVar.h"

#include "ParameterReader.hpp"

// ============================================
// Script to apply sWeights to favoured nTuples
// ============================================
int main(int argc, char * argv[]) {

    // Get input args
    if (argc != 3) {
         std::cout << "Usage: ./WeightTuple <year> <mag>" << std::endl;
         return -1;
    }
    std::string year = argv[1];
    std::string mag = argv[2];

    // Get data
    TFile * file = TFile::Open(("/data/lhcb/users/pullen/B02DKstar/data/twoBody/"
                + year + "_" + mag + "/Kpi_selected.root").c_str(), "READ");
    TTree * tree = (TTree*)file->Get("DecayTree");
    double max = tree->GetEntries();
    RooRealVar Bd_M("Bd_ConsD_MD", "", 5000, 5800);
    RooDataSet * data = new RooDataSet("data", "", tree, RooArgList(Bd_M));

    // ============
    // Make fit PDF
    // ============
    // Read in external parameters
    ParameterReader * pr = new ParameterReader("../Fit_monte_carlo/Results/");
    pr->ReadParameters("signal_Kpi.param", "signal");
    pr->ReadParameters("lowMass.param", "low");
    pr->ReadParameters("DKpipi.param", "DKpipi");
    pr->ReadParameters("rho_all_PIDcut.param", "rho");
    pr->ReadParameters("../../Parameters/gamma_vs_pi.param", "gamma_vs_pi");

    // Make signal shape
    std::map<std::string, RooRealVar*> signal_pars;
    signal_pars["mean"] = new RooRealVar("mean", "", pr->GetValue("mean", "signal") 
            - 10, pr->GetValue("mean", "signal") + 10);
    signal_pars["sigma_L"] = new RooRealVar("sigma_L", "", 
            pr->GetValue("sigma_L", "signal") - 10,
            pr->GetValue("sigma_L", "signal") + 10);
    for (std::string par : {"sigma_ratio", "frac", "alpha_L", "alpha_R", "n_L", "n_R"}) {
        signal_pars[par] = new RooRealVar(par.c_str(), "", pr->GetValue(par, "signal"));
    }
    RooFormulaVar * sigma_R_signal = new RooFormulaVar("sigma_R", "@0 * @1", 
            RooArgList(*signal_pars["sigma_L"], *signal_pars["sigma_ratio"]));
    RooCBShape * cb_L = new RooCBShape("cb_L", "", Bd_M, *signal_pars["mean"],
            *signal_pars["sigma_L"], *signal_pars["alpha_L"], *signal_pars["n_L"]);
    RooCBShape * cb_R = new RooCBShape("cb_R", "", Bd_M, *signal_pars["mean"],
            *sigma_R_signal, *signal_pars["alpha_R"], *signal_pars["n_R"]);
    RooAddPdf * signal_shape = new RooAddPdf("signal_shape", "", 
            RooArgList(*cb_L, *cb_R), RooArgList(*signal_pars["frac"]));

    // Make rho shape
    std::map<std::string, RooRealVar*> rho_pars;
    for (std::string par : {"mean", "sigma_L", "sigma_ratio", "frac", "alpha_L", "alpha_R", "n_L", "n_R"}) {
        rho_pars[par] = new RooRealVar(par.c_str(), "", pr->GetValue(par, "rho"));
    }
    RooFormulaVar * sigma_R_rho = new RooFormulaVar("sigma_R", "@0 * @1", 
            RooArgList(*rho_pars["sigma_L"], *rho_pars["sigma_ratio"]));
    RooCBShape * rho_L = new RooCBShape("rho_L", "", Bd_M, *rho_pars["mean"],
            *rho_pars["sigma_L"], *rho_pars["alpha_L"], *rho_pars["n_L"]);
    RooCBShape * rho_R = new RooCBShape("rho_R", "", Bd_M, *rho_pars["mean"],
            *sigma_R_rho, *rho_pars["alpha_R"], *rho_pars["n_R"]);
    RooAddPdf * rho_shape = new RooAddPdf("rho_shape", "", 
            RooArgList(*rho_L, *rho_R), RooArgList(*rho_pars["frac"]));

    // Make low mass components
    std::map<std::string, RooRealVar*> low_pars;
    for (std::string part : {"pi", "gamma"}) {
        for (std::string point : {"a_", "b_"}) {
            low_pars[point + part] = new RooRealVar((point + part).c_str(), "",
                    pr->GetValue(point + part, "low"));
        }
        for (std::string hel : {"_010", "_101"}) {
            for (std::string par : {"csi_", "sigma_"}) {
                low_pars[par + part + hel] = new RooRealVar((par + part + hel).c_str(),
                        "", pr->GetValue(par + part + hel, "low"));
            }
        }
    }
    low_pars["frac"] = new RooRealVar("frac_low", "", pr->GetValue("frac", "low"));
    low_pars["ratio"] = new RooRealVar("ratio_low", "", pr->GetValue("ratio", "low"));
    low_pars["shiftg"] = new RooRealVar("shiftg", "", 0);
    low_pars["alpha"] = new RooRealVar("alpha", "", 0.7, 0, 1);
    RooHORNSdini * pi_010 = new RooHORNSdini("pi_010", "", Bd_M, *low_pars["a_pi"],
            *low_pars["b_pi"], *low_pars["csi_pi_010"], *low_pars["shiftg"],
            *low_pars["sigma_pi_010"], *low_pars["ratio"], *low_pars["frac"]);
    RooHILLdini * gamma_010 = new RooHILLdini("gamma_010", "", Bd_M, *low_pars["a_gamma"],
            *low_pars["b_gamma"], *low_pars["csi_gamma_010"], *low_pars["shiftg"],
            *low_pars["sigma_gamma_010"], *low_pars["ratio"], *low_pars["frac"]);
    RooHILLdini * pi_101 = new RooHILLdini("pi_101", "", Bd_M, *low_pars["a_pi"],
            *low_pars["b_pi"], *low_pars["csi_pi_101"], *low_pars["shiftg"],
            *low_pars["sigma_pi_101"], *low_pars["ratio"], *low_pars["frac"]);
    RooLITTLEHORNSdini * gamma_101 = new RooLITTLEHORNSdini("gamma_101", "", Bd_M,
            *low_pars["a_gamma"], *low_pars["b_gamma"], *low_pars["csi_gamma_101"], 
            *low_pars["shiftg"], *low_pars["sigma_gamma_101"], *low_pars["ratio"], 
            *low_pars["frac"], *low_pars["shiftg"]);

    // Combine low mass shapes
    std::map<std::string, RooAbsReal*> coeff_pars;
    for (std::string particle : {"_gamma", "_pi"}) {
        for (std::string hel : {"_010", "_101"}) {
            coeff_pars["sel_eff" + particle + hel] = new RooRealVar(
                    ("sel_eff" + particle + hel).c_str(), "",
                    pr->GetValue("sel_eff" + particle + hel, "gamma_vs_pi"));
        }
        coeff_pars["acc_eff" + particle] = new RooRealVar(("acc_eff" + particle).c_str(),
                "", pr-> GetValue("acc_eff" + particle, "gamma_vs_pi"));
        coeff_pars["br" + particle] = new RooRealVar(("br" + particle).c_str(),
                "", pr-> GetValue("br" + particle, "gamma_vs_pi"));
    }
    for (std::string hel : {"010", "101"}) {
        coeff_pars["G_" + hel] = new RooFormulaVar(("G_" + hel).c_str(), "@0 * @1 * @2", 
                RooArgList(*coeff_pars["br_gamma"], *coeff_pars["acc_eff_gamma"], 
                    *coeff_pars["sel_eff_gamma_" + hel]));
        coeff_pars["P_" + hel] = new RooFormulaVar(("P_" + hel).c_str(), "@0 * @1 * @2", 
                RooArgList(*coeff_pars["br_pi"], *coeff_pars["acc_eff_pi"], 
                    *coeff_pars["sel_eff_pi_" + hel]));
        coeff_pars["coeff_gamma_" + hel] = new RooFormulaVar(("coeff_gamma_" + hel).c_str(), 
            "@0/(@0 + @1)", RooArgList(*coeff_pars["G_" + hel], *coeff_pars["P_" + hel]));
        coeff_pars["coeff_pi_" + hel] = new RooFormulaVar(("coeff_pi_" + hel).c_str(), 
            "@0/(@0 + @1)", RooArgList(*coeff_pars["P_" + hel], *coeff_pars["G_" + hel]));
    }
    RooAddPdf * shape_010 = new RooAddPdf("shape_010", "", RooArgList(*gamma_010, *pi_010),
            RooArgList(*coeff_pars["coeff_gamma_010"], *coeff_pars["coeff_pi_010"]));
    RooAddPdf * shape_101 = new RooAddPdf("shape_101", "", RooArgList(*gamma_101, *pi_101),
            RooArgList(*coeff_pars["coeff_gamma_101"], *coeff_pars["coeff_pi_101"]));
    low_pars["alpha"] = new RooRealVar("alpha", "", 0.7, 0, 1);
    RooAddPdf * low_shape = new RooAddPdf("low_shape", "", RooArgList(*shape_010, *shape_101),
            RooArgList(*low_pars["alpha"]));

    // Make DKpipi shape
    std::map<std::string, RooAbsReal*> DKpipi_pars;
    std::vector<std::string> DKpipi_hillPars = {"a", "b", "csi", "sigma", "ratio", 
        "frac"};
    std::vector<std::string> DKpipi_gaussPars = {"mean", "sigma", "f"};
    for (auto par : DKpipi_hillPars) {
        DKpipi_pars[par] = new RooRealVar(("DKpipi_" + par).c_str(), "", pr->GetValue(par, "DKpipi"));
    }
    for (auto par : DKpipi_gaussPars) {
        DKpipi_pars[par + "_gauss1"] = new RooRealVar(("DKpipi_" + par + "_gauss1").c_str(), "", 
                pr->GetValue(par + "_gauss1", "DKpipi"));
        DKpipi_pars[par + "_gauss2"] = new RooRealVar(("DKpipi_" + par + "_gauss2").c_str(), "", 
                pr->GetValue(par + "_gauss2", "DKpipi"));
    }
    RooHILLdini * DKpipi_hill = new RooHILLdini("DKpipi_hill", "", Bd_M, 
            *DKpipi_pars["a"], *DKpipi_pars["b"], *DKpipi_pars["csi"], 
            *low_pars["shiftg"], *DKpipi_pars["sigma"], *DKpipi_pars["ratio"],
            *DKpipi_pars["frac"]);
    RooGaussian * DKpipi_gauss1 = new RooGaussian("DKpipi_gauss1", "", Bd_M,
            *DKpipi_pars["mean_gauss1"], *DKpipi_pars["sigma_gauss1"]);
    RooGaussian * DKpipi_gauss2 = new RooGaussian("DKpipi_gauss2", "", Bd_M,
            *DKpipi_pars["mean_gauss2"], *DKpipi_pars["sigma_gauss2"]);
    RooAddPdf * DKpipi_shape = new RooAddPdf("DKpipi_shape", "", 
            RooArgList(*DKpipi_gauss1, *DKpipi_gauss2, *DKpipi_hill),
            RooArgList(*DKpipi_pars["f_gauss1"], *DKpipi_pars["f_gauss2"]));

    // Make exponential shape
    RooRealVar * slope = new RooRealVar("slope", "", -0.005, -0.01, 0.0);
    RooExponential * comb_shape = new RooExponential("expo", "", Bd_M, *slope);

    // Make yields
    RooRealVar * n_sig = new RooRealVar("n_sig", "", max/3, 0, max);
    RooRealVar * n_rho = new RooRealVar("n_rho", "", max/100, 0, max/20);
    RooRealVar * n_low = new RooRealVar("n_low", "", max/3, 0, max);
    RooRealVar * n_DKpipi = new RooRealVar("n_DKpipi", "", max/3, 0, max);
    RooRealVar * n_comb = new RooRealVar("n_comb", "", max/3, 0, max);

    // Make Kpi shape
    RooAddPdf * model = new RooAddPdf("model", "", RooArgList(*signal_shape,
                *low_shape, *comb_shape, *rho_shape, *DKpipi_shape),
            RooArgList(*n_sig, *n_low, *n_comb, *n_rho, *n_DKpipi));

    // Fit
    RooFitResult * r = model->fitTo(*data, RooFit::Save());
    r->Print();

    // Plot fit to check it looks ok
    RooPlot * plt = Bd_M.frame();
    data->plotOn(plt, RooFit::DrawOption("PZ"), RooFit::LineWidth(1), RooFit::LineColor(kBlack));
    model->plotOn(plt, RooFit::LineColor(kBlue), RooFit::DrawOption("C"), RooFit::LineWidth(2), RooFit::ProjWData(*data));
    model->plotOn(plt, RooFit::LineColor(kRed + 2), RooFit::DrawOption("C"), RooFit::LineWidth(2), RooFit::ProjWData(*data), RooFit::Components("signal_shape"));
    TCanvas * canvas = new TCanvas("canv", "", 900, 600);
    plt->Draw();
    canvas->SaveAs(("Plots/" + year + "_" + mag + "_fit.pdf").c_str());

    // Set non-yield params constant
    low_pars["alpha"]->setConstant();
    signal_pars["mean"]->setConstant();
    signal_pars["sigma_L"]->setConstant();
    slope->setConstant(); 

    // Get sWeights
    RooStats::SPlot * sData = new RooStats::SPlot("sData", "", *data, model, RooArgList(*n_sig, *n_low, *n_comb, *n_rho, *n_DKpipi));
    sData->GetSWeightVars().Print();

    // Make output tree
    TFile * outfile = new TFile(("/data/lhcb/users/pullen/B02DKstar/data/twoBody/"
                + year + "_" + mag + "/Kpi_selected_withWeights.root").c_str(), 
            "RECREATE");
    TTree * outtree = tree->CloneTree(0);
    Double_t sw_signal;
    outtree->Branch("sw_signal", &sw_signal, "sw_signal/D");
    Double_t mass;
    tree->SetBranchAddress("Bd_ConsD_MD", &mass);

    int nentries = tree->GetEntries();
    int n_in_range = 0;
    for (Long64_t n_e = 0; n_e < nentries; n_e++) {
        Long64_t entry = tree->GetEntryNumber(n_e);
        tree->GetEntry(entry);
        // Check mass is in range
        if (5000 < mass && mass < 5800){
            // Extract sWeights
            sw_signal = sData->GetSWeight(n_in_range, "n_sig");
            n_in_range++;
            outtree->Fill();
        }
    }

    // Save
    outtree->Write("DecayTree");
    outfile->Close();

}
