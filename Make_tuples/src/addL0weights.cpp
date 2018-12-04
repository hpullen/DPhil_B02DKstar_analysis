#include <iostream>
#include <vector>

#include "TChain.h"
#include "TFile.h"
#include "TCut.h"

// ============================================================================================
// Script to add weights to account for difference in L0 trigger categories between data and MC
// ============================================================================================
struct bin {
    int number;
    double min_pT;
    double max_pT;
    double min_eta;
    double max_eta;
    double weight;
};

int main(int argc, char * argv[]) {

    // Get mode
    TString mode = "Kpi";
    if (argc == 2) {
        mode = argv[1];
    }
    TString bod = (mode == "Kpipipi" || mode == "pipipipi") ? "fourBody" : "twoBody";

    // Get years
    std::vector<TString> years = {"2016"};
    if (mode != "pipipipi") years.push_back("2012");
    if (mode != "pipipipi" && mode != "Kpipipi") years.push_back("2015");
    if (mode == "Kpi") years.push_back("2011");

    // Loop through years and polarities
    for (TString year : years) {
        for (TString mag : {"_up", "_down"}) {

            // Open MC input file 
            TString data_dir = "/data/lhcb/users/pullen/B02DKstar/";
            TFile * infile = TFile::Open(data_dir + "MC/" + bod + "/" + mode + "/" + year + mag
                    + "/" + mode + "_withBDTG.root", "READ");
            TTree * intree = (TTree*)infile->Get("DecayTree");

            // Make output tree
            TFile * outfile = TFile::Open(data_dir + "/MC/" + bod + "/" + mode + "/" + year + mag
                    + "/" + mode + "_withBDTG_withL0weights.root", "RECREATE");
            TTree * outtree = intree->CloneTree(0);

            // Open reference data and MC files
            TChain * mc_tree = new TChain("DecayTree");
            TChain * data_tree = new TChain("DecayTree");
            for (TString mag_in : {"_up", "_down"}) {
                mc_tree->Add(data_dir + "/MC/" + bod + "/" + mode + "/" + year + mag_in + "/" + mode + "_selected.root");
                data_tree->Add(data_dir + "/data/" + bod + "/" + year + mag_in + "/" + mode + "_selected.root");
            }

            // Get weights for each type
            TString cut_TOS = "Bd_L0HadronDecision_TOS && !Bd_L0Global_TIS";
            TString data_cut_TOS = "abs(Bd_ConsD_M - 5279.81) < 25 && " + cut_TOS;
            TString cut_TIS = "Bd_L0Global_TIS";
            TString data_cut_TIS = "abs(Bd_ConsD_M - 5279.81) < 25 && " + cut_TIS;
            double data_tot = data_tree->GetEntries("abs(Bd_ConsD_M - 5279.81) < 25");
            double data_TOS = data_tree->GetEntries(data_cut_TOS);
            double data_TIS = data_tree->GetEntries(data_cut_TIS);
            double data_frac_TOS = data_TOS/data_tot;
            double data_frac_TIS = data_TIS/data_tot;
            double mc_tot = mc_tree->GetEntries();
            double mc_TOS = mc_tree->GetEntries(cut_TOS);
            double mc_TIS = mc_tree->GetEntries(cut_TIS);
            double mc_frac_TOS = mc_TOS/mc_tot;
            double mc_frac_TIS = mc_TIS/mc_tot;
            double weight_TOS = data_frac_TOS/mc_frac_TOS;
            double weight_TIS = data_frac_TIS/mc_frac_TIS;

            // Make branch for weights
            double L0_weight = 0;
            outtree->Branch("L0_weight", &L0_weight, "L0_weight/D");

            // Branches for trigger in input tree
            bool input_TOS;
            bool input_TIS;
            intree->SetBranchAddress("Bd_L0HadronDecision_TOS", &input_TOS);
            intree->SetBranchAddress("Bd_L0Global_TIS", &input_TIS);

            // Get pT/eta weights

            // Fill
            for (int i = 0; i < intree->GetEntries(); i++) {
                intree->GetEntry(i);
                L0_weight = ((input_TOS && !input_TIS) * weight_TOS) + (input_TIS * weight_TIS);
                outtree->Fill();
            }

            // Save
            outfile->cd();
            outtree->Write("DecayTree");
            outfile->Close();
            infile->Close();
        }
    }
}
