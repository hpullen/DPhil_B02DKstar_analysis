#include <iostream>
#include <string>
#include <cmath>

#include "TChain.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TTree.h"
#include "TCut.h"
#include "TEventList.h"

#include "CutReader.hpp"


// ====================================================
// Make fully selected nTuple (except BDT cut) for data
// ====================================================
int main(int argc, char * argv[]) {

    // Check for correct number of args
    if (argc != 4 && argc != 5) {
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
    std::string cat = "";
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
    } else {
        // Check if category is low mass or rho
        if (type.find("Bs") != std::string::npos) cat = "Bs";
        if (type.find("lowMass") != std::string::npos) cat = "low";
        if (type.find("rho") != std::string::npos) cat = "rho";
        if (type.find("rho_Kpipipi") != std::string::npos) mode = "Kpipipi";
        if (type.find("DKpipi") != std::string::npos) cat = "DKpipi";
        if (type.find("Kpi_sim09b") != std::string::npos) cat = "signal";
        if (type.find("Lambda_b") != std::string::npos) cat = "lambda";
        if (type.find("Ed") != std::string::npos) cat = "signal";
    }

    // Name of tree
    std::string treename = "DecayTree";

    // Path to input file
    std::string inputPath = "/data/lhcb/users/pullen/B02DKstar/MC/" + type + "/" + 
        year + "_" + mag + "/";
    std::string inputFile;
    if (cat == "signal" || cat == "Bs" || cat == "low" || cat == "rho") {
        inputFile = inputPath + mode + "_withBDTG_withWeights.root";
    } else {
        inputFile = inputPath + mode + "_withBDTG.root";
    }
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
    CutReader * cr = new CutReader(mode);
    std::vector<std::string> to_ignore = {"PID", "hasRich"};
    TCut cut = cr->GetCutExcept(to_ignore);

    // Add truth-matching cut
    if (cat == "signal") {
        std::cout << "Mode is signal. Applying BKGCAT == 0 cut.";
        cut += "Bd_BKGCAT == 0";
    } else if (cat == "low") {
        std::cout << "Mode is low mass. Applying BKGCAT == 50 cut.";
        cut += "Bd_BKGCAT == 50";
    } else if (cat == "Bs") {
        std::cout << "Mode is Bs. Applying BKGCAT == 20 cut.";
        cut += "Bd_BKGCAT == 20";
    } else if (cat == "rho") {
        std::cout << "Mode is rho. Applying BKGCAT == 30 cut.";
        cut += "Bd_BKGCAT == 30";
    } else if (cat == "DKpipi" || cat == "lambda") {
        std::cout << "Applying no BKGCAT cut." << std::endl;
    } else {
        std::cout << "Category not recognised!" << std::endl;
        return -1;
    }

    // Edit KstarK_ID to KstarK_TRUEID
    std::string cut_orig = std::string(cut);
    std::string find = "KstarK_ID";
    while (cut_orig.find(find) != std::string::npos) {
        std::string before = cut_orig.substr(0, cut_orig.find(find));
        std::string after = cut_orig.substr(cut_orig.find(find) 
                + find.length(), std::string::npos);
        cut_orig = before + "KstarK_TRUEID" + after;
    }
    cut = cut_orig.c_str();

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
