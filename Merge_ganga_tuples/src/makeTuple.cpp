#include <iostream>
#include <vector>
#include <string>
#include <cmath>

#include "TChain.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TTree.h"
#include "TCut.h"
#include "TEventList.h"

// Helper function for calculating helicity angle
inline double Product(const TLorentzVector& p1, const TLorentzVector& p2,
        const TLorentzVector& pX) {
    return - p1.Dot(p2) + p1.Dot(pX) * p2.Dot(pX) / pX.Dot(pX);
}

// Helper function for setting up Lorentz vector branch
void LorentzVectorBranch(TTree* tree, TLorentzVector& vec,
        const std::string& name) {
    tree->SetBranchAddress((name + "_PE").c_str(), &vec[3]);
    tree->SetBranchAddress((name + "_PX").c_str(), &vec[0]);
    tree->SetBranchAddress((name + "_PY").c_str(), &vec[1]);
    tree->SetBranchAddress((name + "_PZ").c_str(), &vec[2]);
}



// ======================================================================================
// Script to combine multiple nTuples and apply trigger, K* mass, and helicity angle cuts
// ======================================================================================
int main(int argc, char * argv[]) {

    // Check for correct number of args
    if (argc < 6) {
        std::cout << "Usage: ./MakeTuple <output-dir> <year> <mode> <treename> "
            "<input-files> ..." << std::endl;
        return -1;
    }

    // Location for output tuple 
    std::string output_dir = argv[1];

    // Year (2011, 2012, 2015, 2016)
    std::string year = argv[2];
    std::cout << "Year: " << year << std::endl;

    // D0 decay mode
    std::string mode = argv[3];
    std::cout << "Mode: " << mode << std::endl;

    // Name of tree
    std::string treename = argv[4];

    // Get list of input files
    std::vector<std::string> input_files;
    for (int i = 5; i < argc; i++) {
        input_files.push_back(argv[i]);
        std::cout << "Adding file: " << argv[i] << std::endl;
    }
    std::cout << "Combining " << input_files.size() << " files." << std::endl;

    // Make TChain and add files
    TChain * tree = new TChain(treename.c_str());
    for (auto file : input_files) {
        tree->Add(file.c_str());
    }

    // Make output file and tree
    std::string outputFile = "/data/lhcb/users/pullen/B02DKstar/" + output_dir + 
        "/" + mode + ".root";
    std::cout << "Output will be saved to " << outputFile << std::endl;
    TFile * new_file = TFile::Open(outputFile.c_str(), "RECREATE");
    TTree * new_tree = (TTree*)tree->CloneTree(0);

    // Variables needed for helicity angle calculation
    TLorentzVector KstarK_P, KstarPi_P, D0_P;
    LorentzVectorBranch(tree, KstarK_P, "KstarK");
    LorentzVectorBranch(tree, KstarPi_P, "KstarPi");
    LorentzVectorBranch(tree, D0_P, "D0");

    // New branch to hold helicity angle
    double helicityAngle;
    new_tree->Branch("Kstar_helicity_angle", &helicityAngle, 
            "Kstar_helicity_angle/D");

    // Make a cut string for K*0 mass cut
    TCut cut = "abs(895.55 - Kstar_M) < 50.0";

    // Add trigger cuts based on year
    cut += "Bd_L0Global_TIS || Bd_L0HadronDecision_TOS";
    if (year == "2011" || year == "2012") {
        cut += "Bd_Hlt1TrackAllL0Decision_TOS";
        cut += "Bd_Hlt2Topo2BodyBBDTDecision_TOS || "
            "Bd_Hlt2Topo3BodyBBDTDecision_TOS || Bd_Hlt2Topo4BodyBBDTDecision_TOS";
    } else {
        cut += "Bd_Hlt1TrackMVADecision_TOS || Bd_Hlt1TwoTrackMVADecision_TOS";
        cut += "Bd_Hlt2Topo2BodyDecision_TOS || Bd_Hlt2Topo3BodyDecision_TOS || "
            "Bd_Hlt2Topo4BodyDecision_TOS";
    }

    // Select events passing mass and trigger cuts
    tree->Draw(">>elist", cut);
    TEventList * eventList = (TEventList*)gDirectory->Get("elist");

    // Get number of entries and make counting variables
    int n_entries = eventList->GetN();
    std::cout << "Looping over " << n_entries << " entries." << std::endl;
    int n_pass = 0; 

    // Loop over entries
    for (int i = 0; i < n_entries; i++) {

        // Get the entry and occasionally print info
        tree->GetEntry(eventList->GetEntry(i));
        if (i % 100000 == 0) {
            std::cout << "Processing event " << i << std::endl;
        }

        // Work out helicity angle
        helicityAngle = Product(KstarK_P, D0_P, KstarK_P + KstarPi_P) / 
            sqrt(Product(KstarK_P, KstarK_P, KstarK_P + KstarPi_P) * 
                    Product(D0_P, D0_P, KstarK_P + KstarPi_P));

        // Ignore this event if abs(helicity) < 0.4
        if (std::abs(helicityAngle) < 0.4) continue;

        // Fill the new tree
        new_tree->Fill();
        n_pass++;
    }

    std::cout << "Filled new tree with " << n_pass << " entries.\n" << std::endl;
    
    // Save
    new_tree->Write();
    new_file->Close();
    delete tree;

    return 0;
}
