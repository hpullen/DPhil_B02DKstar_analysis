#include <iostream>
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


// ====================================================
// Make fully selected nTuple (except BDT cut) for data
// ====================================================
int main(int argc, char * argv[]) {

    // Check for correct number of args
    if (argc != 4) {
        std::cout << "Usage: ./MakeSelectedTuple <2011/2012/2015/2016> <up/down> " <<
            "<D0mode>" << std::endl;
        return -1;
    }

    // Year (2011, 2012, 2015, 2016)
    std::string year = argv[1];

    // Polarity (up vs. down)
    std::string mag = argv[2];

    // D0 decay mode
    std::string mode = argv[3];

    // 2-body vs. 4-body
    std::string bodies = "twoBody";
    if (mode == "Kpipipi" || mode == "piKpipi" || mode == "pipipipi") {
        bodies = "fourBody";
    }

    // Name of tree
    std::string treename = "DecayTree";

    // Path to input file
    std::string inputPath = "/data/lhcb/users/pullen/B02DKstar/data/" + bodies + 
        "/" + year + "_" + mag + "/";
    std::string inputFile = inputPath + mode + "_withVars_withCuts.root";
    std::cout << "Taking input from tree " << treename << " in file " <<
        inputFile << std::endl;

    // Read input file and tree
    TFile * file = TFile::Open(inputFile.c_str(), "READ");
    TTree * tree = (TTree*)file->Get(treename.c_str());

    // Turn off unnecessary branches
    tree->SetBranchStatus("*", 0);
    tree->SetBranchStatus("Bd_ConsD_MD", 1);
    tree->SetBranchStatus("KstarK_ID", 1);
    tree->SetBranchStatus("D0_FDS", 1);
    tree->SetBranchStatus("D0_M", 1);
    tree->SetBranchStatus("eventNumberD", 1);
    tree->SetBranchStatus("PolarityD", 1);

    // Turn on BDT branches
    std::string BDT_mode = mode;
    if (mode == "piK") BDT_mode = "Kpi";
    if (mode == "piKpipi") BDT_mode = "Kpipipi";
    tree->SetBranchStatus(("BDTG_" + BDT_mode + "_run2").c_str(), 1);

    // Turn on PID branches
    tree->SetBranchStatus("KstarK_PIDK", 1);
    tree->SetBranchStatus("KstarPi_PIDK", 1);
    if (mode == "Kpi" || mode == "piK" || mode == "Kpipipi" || mode == "piKpipi") {
        tree->SetBranchStatus("D0K_PIDK", 1);
        tree->SetBranchStatus("D0Pi_PIDK", 1);
    } else if (mode == "KK") {
        tree->SetBranchStatus("D0Kplus_PIDK", 1);
        tree->SetBranchStatus("D0Kminus_PIDK", 1);
    } else if (mode == "pipipipi") {
        tree->SetBranchStatus("D0PiPlus1_PIDK", 1);
        tree->SetBranchStatus("D0PiPlus2_PIDK", 1);
        tree->SetBranchStatus("D0PiMinus1_PIDK", 1);
        tree->SetBranchStatus("D0PiMinus2_PIDK", 1);
    }
    if (mode == "Kpipipi" || mode == "piKpipi" || mode == "pipi") {
        tree->SetBranchStatus("D0PiPlus_PIDK", 1);
        tree->SetBranchStatus("D0PiMinus_PIDK", 1);
    }

    // Turn on double mis-ID branches
    if (mode == "Kpi" || mode == "piK" || mode == "Kpipipi" || mode == "piKpipi") {
        tree->SetBranchStatus("D0_deltaM_doubleSwap", 1);
        if (mode == "Kpipipi" || mode == "piKpipi") {
            tree->SetBranchStatus("D0_deltaM_doubleSwap_otherPion", 1);
        }
    }

    // Make output file and tree
    // Tuple with selection cuts
    std::string outputFile = inputPath + mode + "_selected.root";
    std::cout << "Fully selected output will be saved to " << outputFile << std::endl;
    TFile * new_file = TFile::Open(outputFile.c_str(), "RECREATE");
    TTree * new_tree = (TTree*)tree->CloneTree(0);

    // Make cut
    TCut cut = "abs(D0_M - 1864.83) < 25 && "
               "Bd_ConsD_MD > 5000 && "
               "Bd_ConsD_MD < 5800 && "
               "D0_FDS > 2 && "
               "KstarK_PIDK > 3 && "
               "KstarPi_PIDK < -1";

    // Add mode-dependent cuts
    if (mode == "Kpi" || mode == "piK" || mode == "Kpipipi" || mode == "piKpipi") {

        // ADS modes have double mis-ID veto and cuts on D0 daughter kaon and pion
        cut += "D0_deltaM_doubleSwap > 15 && "
               "D0K_PIDK > 1 &&"
               "D0Pi_PIDK < -1";

        if (mode == "Kpipipi" || mode == "piKpipi") {

            // 4-body ADS have extra veto
            cut += "D0_deltaM_doubleSwap_otherPion > 15";

            // Extra PID cut on second opposite-sign pion
            if (mode == "Kpipipi") {
                cut += "(KstarK_ID < 0)*(D0PiPlus_PIDK < -1) + "
                       "(KstarK_ID > 0)*(D0PiMinus_PIDK < -1)";
            } else {
                cut += "(KstarK_ID < 0)*(D0PiMinus_PIDK < -1) + "
                       "(KstarK_ID > 0)*(D0PiPlus_PIDK < -1)";
            }
        }
    } else if (mode == "KK") {
        cut += "D0Kplus_PIDK > 1 && D0Kminus_PIDK > 1";
    } else if (mode == "pipi") {
        cut += "D0PiPlus_PIDK < -1 && D0PiMinus_PIDK < -1";
    } else if (mode == "pipipipi") {

        // Cut on the two pions with same sign as K*0 daughter kaon
        cut += "(KstarK_ID < 0)*(D0PiMinus1_PIDK < -1 && D0PiMinus2_PIDK < -1) + "
               "(KstarK_ID > 0)*(D0PiPlus1_PIDK < -1 && D0PiPlus2_PIDK < -1)";
    }

    // Select events passing mass/trigger/loose BDT cuts
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
    new_tree->Write();
    new_file->Close();
    delete tree;
    file->Close();
    delete file;

    return 0;
}
