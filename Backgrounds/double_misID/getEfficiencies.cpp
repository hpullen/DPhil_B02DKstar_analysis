#include <iostream>
#include <fstream>

#include "TChain.h"
#include "TCut.h"

// ==================================================
// Script to get efficiencies of D0 mass cut and veto
// ==================================================
int main(int argc, char * argv[]) {

    // Get input args
    if (argc != 3) {
        std::cout << "Usage: ./GetEfficiencies <mode> <run>" << std::endl;
        return -1;
    }
    TString mode = argv[1];
    std::string run = argv[2];
    if (mode != "Kpi" && mode != "Kpipipi") {
        std::cout << "Invalid mode " << mode << ". Choose Kpi or Kpipipi." << std::endl;
        return -1;
    }
    if (run != "1" && run != "2") {
        std::cout << "Invalid run " << mode << ". Choose 1 or 2." << std::endl;
        return -1;
    }

    // Load MC
    TChain * tree = new TChain("DecayTree");
    TString bod = (mode == "Kpi") ? "twoBody" : "fourBody";
    std::vector<TString> mags = {"up", "down"};
    std::vector<TString> years;
    if (run == "1") {
        years = {"2012"};
        if (mode == "Kpi") {
            years.push_back("2011");
        }
    } else {
        years = {"2016"};
        if (mode == "Kpi") {
            years.push_back("2015");
        }
    }
    for (auto mag : mags) {
        for (auto year : years) {
            tree->Add("/data/lhcb/users/pullen/B02DKstar/MC/" + bod + "/" + 
                    mode + "/" + year + "_" + mag + "/" + mode + "_withBDTG.root");
        }
    }

    // Get total entries
    TCut cut = "Bd_BKGCAT == 0";
    double total_entries = tree->GetEntries(cut);

    // Get D0_M cut efficiency
    double D0_M_entries_true = tree->GetEntries("abs(D0_M - 1864.84) < 25" + cut);
    double D0_M_entries_swap; 
    if (mode == "Kpi") {
        D0_M_entries_swap = tree->GetEntries("abs(D0_M_doubleSwap - 1864.84) < 25" + cut);
    } else {
        D0_M_entries_swap = tree->GetEntries("abs(D0_M_doubleSwap_high - 1864.84) < 25" + cut);
    }
    double D0_M_eff_true = D0_M_entries_true/total_entries;
    double D0_M_eff_swap = D0_M_entries_swap/total_entries;
    double D0_M_err_true = (1/total_entries) * sqrt(D0_M_entries_true * 
            (1 - D0_M_entries_true / total_entries));
    double D0_M_err_swap = (1/total_entries) * sqrt(D0_M_entries_swap * 
            (1 - D0_M_entries_swap / total_entries));

    // Get veto efficiency
    double veto_entries_true;
    if (mode == "Kpi") {
        veto_entries_true = tree->GetEntries("abs(D0_M_doubleSwap - 1864.84) > 15" + cut);
    } else {
        veto_entries_true = tree->GetEntries("abs(D0_M_doubleSwap_low - 1864.84) > 15 && abs(D0_M_doubleSwap_high - 1864.84) > 15" + cut);
    }
    double veto_entries_swap = tree->GetEntries("abs(D0_M - 1864.84) > 15" + cut);
    double veto_eff_true = veto_entries_true/total_entries;
    double veto_eff_swap = veto_entries_swap/total_entries;
    double veto_err_true = (1/total_entries) * sqrt(veto_entries_true * 
            (1 - veto_entries_true / total_entries));
    double veto_err_swap = (1/total_entries) * sqrt(veto_entries_swap * 
            (1 - veto_entries_swap / total_entries));

    // Print to file
    std::ofstream file("Efficiencies/" + std::string(mode) + "_run" + run + ".param");
    file << "D0_M_true " << D0_M_eff_true << " " << D0_M_err_true << std::endl;
    file << "D0_M_swap " << D0_M_eff_swap << " " << D0_M_err_swap << std::endl;
    file << "veto_true " << veto_eff_true << " " << veto_err_true << std::endl;
    file << "veto_swap " << veto_eff_swap << " " << veto_err_swap << std::endl;
    file.close();
}
