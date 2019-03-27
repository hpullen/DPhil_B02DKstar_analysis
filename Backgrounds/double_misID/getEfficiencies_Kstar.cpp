#include <iostream>
#include <fstream>

#include "TChain.h"
#include "TCut.h"

// ==================================================
// Script to get efficiencies of D0 mass cut and veto
// ==================================================
int main(int argc, char * argv[]) {

    // Get input args
    // if (argc != 2) {
        // std::cout << "Usage: ./GetEfficiencies <run>" << std::endl;
        // return -1;
    // }
    TString mode = "Kpi";
    std::string run = "2";

    // Load MC
    TChain * tree = new TChain("Tuple_Kpi/DecayTree");
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
                    mode + "/" + year + "_" + mag + "/Total.root");
        }
    }

    // Get total entries
    TCut cut = "Bd_BKGCAT == 0";
    double total_entries = tree->GetEntries(cut);

    // Get Kstar_M cut efficiency
    double Kstar_M_entries_true = tree->GetEntries("abs(Kstar_M - 895.5) > 25" + cut);
    double Kstar_M_entries_swap; 
    std::string swapped_var = "sqrt(pow(sqrt(pow(KstarK_P,2) + pow(139.6,2)) + "
        "sqrt(pow(KstarPi_P,2) + pow(493.7,2)),2)";
    for (std::string mom : {"PX", "PY", "PZ"}) {
        swapped_var += " - pow(KstarK_" + mom + " + KstarPi_" + mom + ",2)";
    }
    swapped_var += ")";
    std::string swapped_mass_cut_str = "abs(" + swapped_var + " - 895.5) > 25";
    TCut swapped_mass_cut = swapped_mass_cut_str.c_str();
    Kstar_M_entries_swap = tree->GetEntries(swapped_mass_cut + cut);
    double Kstar_M_eff_true = Kstar_M_entries_true/total_entries;
    double Kstar_M_eff_swap = Kstar_M_entries_swap/total_entries;
    double Kstar_M_err_true = (1/total_entries) * sqrt(Kstar_M_entries_true * 
            (1 - Kstar_M_entries_true / total_entries));
    double Kstar_M_err_swap = (1/total_entries) * sqrt(Kstar_M_entries_swap * 
            (1 - Kstar_M_entries_swap / total_entries));

    // Print to file
    std::ofstream file("Efficiencies/Kstar_" + std::string(mode) + "_run" + run + "_veto.param");
    file << "Kstar_M_true " << Kstar_M_eff_true << " " << Kstar_M_err_true << std::endl;
    file << "Kstar_M_swap " << Kstar_M_eff_swap << " " << Kstar_M_err_swap << std::endl;
    file.close();
}
