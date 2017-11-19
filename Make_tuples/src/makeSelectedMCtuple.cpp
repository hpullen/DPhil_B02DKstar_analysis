#include <iostream>
#include <string>
#include <cmath>

#include "TChain.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TTree.h"
#include "TCut.h"
#include "TEventList.h"


// ====================================================
// Make fully selected nTuple (except BDT cut) for data
// ====================================================
int main(int argc, char * argv[]) {

    // Check for correct number of args
    if (argc != 4) {
        std::cout << "Usage: ./MakeSelectedMCtuple <type> <year> <mag>" << std::endl;
        return -1;
    }

    // Monte Carlo type
    std::string type = argv[1];

    // Year (2011, 2012, 2015, 2016)
    std::string year = argv[2];

    // Polarity (up vs. down)
    std::string mag = argv[3];

    // Infer D0 mode/background type/number of bodies from type
    // Defaults (low mass Kpi background)
    std::string mode = "Kpi";
    std::string cat = "low";
    std::string bodies = "twoBody";

    // Check if MC is for signal rather than background
    if (type.find("backgrounds") == std::string::npos) {
        cat = "signal";

        // Check if signal is four body
        if (type.find("fourBody") != std::string::npos) {
            bodies = "fourBody";
            if (type.find("Kpipipi") != std::string::npos) mode = "Kpipipi";
            else if (type.find("pipipipi") != std::string::npos) mode = "pipipipi";
        } else {
            if (type.find("KK") != std::string::npos) mode = "KK";
            else if (type.find("pipi") != std::string::npos) mode = "pipi";
        }
    }
    std::string bdt_mode = mode;
    if (mode == "piK") bdt_mode = "Kpi";
    else if (mode == "piKpipi") bdt_mode = "Kpipipi";

    // Name of tree
    std::string treename = "DecayTree";

    // Path to input file
    std::string inputPath = "/data/lhcb/users/pullen/B02DKstar/MC/" + type + "/" + 
        year + "_" + mag + "/";
    std::string inputFile = inputPath + mode + "_withBDTG.root";
    std::cout << "Taking input from tree " << treename << " in file " <<
        inputFile << std::endl;

    // Read input file and tree
    TFile * file = TFile::Open(inputFile.c_str(), "READ");
    TTree * tree = (TTree*)file->Get(treename.c_str());

    // Make output file and tree
    std::string outputFile = inputPath + mode + "_selected.root";
    std::cout << "Fully selected output will be saved to " << outputFile << std::endl;
    TFile * new_file = TFile::Open(outputFile.c_str(), "RECREATE");
    TTree * new_tree = (TTree*)tree->CloneTree(0);

    // Make cut
    TCut cut = "abs(D0_M - 1864.83) < 25 && "
               "Bd_ConsD_MD > 5000 && "
               "Bd_ConsD_MD < 5800 && "
               "D0_FDS > 2";

    // Add BDT cut
    cut += ("BDTG_" + bdt_mode + "_run2 > 0.5").c_str();

    // Add mis-ID cuts
    if (mode == "Kpi" || mode == "piK" || mode == "Kpipipi" || mode == "piKpipi") {
        cut += "D0_deltaM_doubleSwap";
        if (mode == "Kpipipi" || mode == "piKpipi") {
            cut += "D0_deltaM_doubleSwap_otherPion > 15";
        }
    } 

    // Add truth-matching cut
    if (cat == "signal") {
        cut += "Bd_BKGCAT == 0";
    } else {
        cut += "Bd_BKGCAT == 50";
    }

    // Select events passing selection
    tree->Draw(">>elist", cut);
    TEventList * eventList = (TEventList*)gDirectory->Get("elist");

    // Get number of entries and make counting variables
    int n_entries = eventList->GetN();
    std::cout << "Looping over " << n_entries << " entries." << std::endl;
    int n_pass = 0; 

    // Loop over entries
    for (int i = 0; i < n_entries; i++) {

        // Get the entry
        tree->GetEntry(eventList->GetEntry(i));

        // Fill the new tree
        new_tree->Fill();
        n_pass++;
    }

    std::cout << "Filled new tree with " << n_pass << " entries.\n" << std::endl;
    
    // Save
    new_file->cd();
    new_tree->Write();
    new_file->Close();
    delete tree;
    file->Close();
    delete file;

    return 0;
}
