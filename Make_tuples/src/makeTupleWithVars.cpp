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


int main(int argc, char * argv[]) {

    // Check for correct number of args
    if (argc != 6 && argc != 4){
        std::cout << "Usage: ./MakeSelectedTuple <data/MC> <2011/2012/2015/2016> "
            "<up/down> <twoBody/fourBody> <D0mode>" << std::endl;
        std::cout << "or..." << std::endl;
        std::cout << "./MakeSelectedTuple <directory-path> <2011/2012/2015/2016> "
            "<up/down>" << std::endl;
        return -1;
    }

    // File type (data vs. MC)
    std::string type = ((argc == 6) ? argv[1] : "MC");

    // Year (2011, 2012, 2015, 2016)
    std::string year = argv[2];

    // Polarity (up vs. down)
    std::string mag = argv[3];

    // 2-body vs. 4-body
    std::string bodies = ((argc == 6) ? argv[4] : "twoBody");

    // D0 decay mode
    std::string mode = ((argc == 6) ? argv[5] : "Kpi");

    // Name of tree
    std::string treename = "DecayTree";

    // Path to input file
    std::string inputPath;
    if (argc == 6) {
        inputPath = "/data/lhcb/users/pullen/B02DKstar/" + type + "/" +
            bodies + "/";
        if (type == "MC") {
            inputPath += mode + "/";
        }
    } else {
        inputPath = std::string(argv[1]) + "/";
    }
    inputPath += year + "_" + mag + "/";
    std::string inputFile = inputPath + mode + "_withBDTG.root";
    std::cout << "Taking input from tree " << treename << " in file " <<
        inputFile << std::endl;

    // Read input file and tree
    TFile * file = TFile::Open(inputFile.c_str(), "READ");
    TTree * tree = (TTree*)file->Get(treename.c_str());

    // Make output file and tree
    // Tuple with selection cuts
    std::string outputFile = inputPath + mode + "_withVars_withCuts.root";
    std::cout << "Fully selected output will be saved to " << outputFile << std::endl;
    TFile * new_file = TFile::Open(outputFile.c_str(), "RECREATE");
    TTree * new_tree = (TTree*)tree->CloneTree(0);

    // Variables needed for D0 flight distance calculation
    double D0_ENDVERTEX_Z;
    double D0_ENDVERTEX_ZERR;
    double Bd_ENDVERTEX_Z;
    double Bd_ENDVERTEX_ZERR;
    tree->SetBranchAddress("D0_ENDVERTEX_Z", &D0_ENDVERTEX_Z);
    tree->SetBranchAddress("D0_ENDVERTEX_ZERR", &D0_ENDVERTEX_ZERR);
    tree->SetBranchAddress("Bd_ENDVERTEX_Z", &Bd_ENDVERTEX_Z);
    tree->SetBranchAddress("Bd_ENDVERTEX_ZERR", &Bd_ENDVERTEX_ZERR);

    // New branches to hold D0 FD
    double D0_FD;
    double D0_FD_ERR;
    double D0_FDS;
    new_tree->Branch("D0_FD", &D0_FD, "D0_FD/D");
    new_tree->Branch("D0_FD_ERR", &D0_FD_ERR, "D0_FD_ERR/D");
    new_tree->Branch("D0_FDS", &D0_FDS, "D0_FDS/D");

    // EventNumber cast as a double
    unsigned long long eventNumber;
    double eventNumberD;
    tree->SetBranchAddress("eventNumber", &eventNumber);
    new_tree->Branch("eventNumberD", &eventNumberD, "eventNumberD/D");

    // Magentic polarity cast as a double
    short polarity;
    double polarityD;
    tree->SetBranchAddress("Polarity", &polarity);
    new_tree->Branch("PolarityD", &polarityD, "PolarityD/D");

    // B0 decaytreefitter mass cast as a double
    float Bd_ConsD_M;
    double Bd_ConsD_MD;
    tree->SetBranchAddress("Bd_ConsD_M", &Bd_ConsD_M);
    new_tree->Branch("Bd_ConsD_MD", &Bd_ConsD_MD, "Bd_ConsD_MD/D");

    // Variables needed for double mass swap calculation
    const double mass_pi = 139.57;
    const double mass_K = 493.68;
    TLorentzVector v_D0K_P, v_D0Pi_P;

    // New vectors and variable for results
    TLorentzVector v_K_as_pi, v_pi_as_K, v_D0_doubleSwap;
    double D0_M_doubleSwap;
    double D0_deltaM_doubleSwap;
    const double D0_M_PDG = 1864.84;
    
    // Set up the branches if mode is an ADS mode
    bool is_ADS = (mode == "Kpi" || mode == "piK" || mode == "Kpipipi" 
        || mode == "piKpipi");
    if (is_ADS) {
        LorentzVectorBranch(tree, v_D0K_P, "D0K");
        LorentzVectorBranch(tree, v_D0Pi_P, "D0Pi");
        if (bodies == "twoBody") {
            new_tree->Branch("D0_M_doubleSwap", &D0_M_doubleSwap, "D0_M_doubleSwap/D");
            new_tree->Branch("D0_deltaM_doubleSwap", &D0_deltaM_doubleSwap, 
                    "D0_deltaM_deltadoubleSwap/D");
        }
    }

    // Variables needed for 4-body double mass swap calculation 
    TLorentzVector v_D0PiPlus_P, v_D0PiMinus_P;
    int KstarK_ID;

    // New vectors and variable for results
    TLorentzVector v_low_pi_as_K, v_high_pi_as_K, v_D0_doubleSwap_low, 
                   v_D0_doubleSwap_high;
    double D0_M_doubleSwap_low;
    double D0_deltaM_doubleSwap_low;
    double D0_M_doubleSwap_high;
    double D0_deltaM_doubleSwap_high;

    // Set up the branches if mode is 4-body ADS
    if (is_ADS && bodies == "fourBody") {
        LorentzVectorBranch(tree, v_D0PiPlus_P, "D0PiPlus");
        LorentzVectorBranch(tree, v_D0PiMinus_P, "D0PiMinus");
        if (type == "MC") {
            tree->SetBranchAddress("KstarK_TRUEID", &KstarK_ID);
        } else {
            tree->SetBranchAddress("KstarK_ID", &KstarK_ID);
        }
        new_tree->Branch("D0_M_doubleSwap_low", &D0_M_doubleSwap_low, "D0_M_doubleSwap_low/D");
        new_tree->Branch("D0_deltaM_doubleSwap_low", &D0_deltaM_doubleSwap_low, "D0_deltaM_doubleSwap_low/D");
        new_tree->Branch("D0_M_doubleSwap_high", &D0_M_doubleSwap_high, "D0_M_doubleSwap_high/D");
        new_tree->Branch("D0_deltaM_doubleSwap_high", &D0_deltaM_doubleSwap_high, "D0_deltaM_doubleSwap_high/D");
    }

    // Add a loose BDT cut
    std::string BDT_mode = mode;
    if (mode == "piK") BDT_mode = "Kpi";
    if (mode == "piKpipi") BDT_mode = "Kpipipi";
    TCut cut = ("BDTG_" + BDT_mode + "_run2 > 0.2").c_str();

    // Select events passing mass/trigger/loose BDT cuts
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

        // Cast event number and polarity as doubles
        eventNumberD = (double)eventNumber;
        polarityD = (double)polarity;
        Bd_ConsD_MD = (double)Bd_ConsD_M;

        // Work out D0 flight distance variables
        D0_FD = D0_ENDVERTEX_Z - Bd_ENDVERTEX_Z;
        D0_FD_ERR = sqrt(pow(D0_ENDVERTEX_ZERR, 2) + pow(Bd_ENDVERTEX_ZERR, 2));
        D0_FDS = D0_FD / D0_FD_ERR;

        // Work out double swapped mass for ADS modes
        if (is_ADS) {

            // Calculate vector for kaon with pi mass
            v_K_as_pi.SetPxPyPzE(v_D0K_P[0], v_D0K_P[1], v_D0K_P[2],
                    sqrt(pow(v_D0K_P.P(), 2) + pow(mass_pi, 2)));

            // Two body: swap D0 daughter kaon and pion masses
            if (bodies == "twoBody") {

                // Calculate vectors for pion with K mass
                v_pi_as_K.SetPxPyPzE(v_D0Pi_P[0], v_D0Pi_P[1], v_D0Pi_P[2],
                        sqrt(pow(v_D0Pi_P.P(), 2) + pow(mass_K, 2)));

                // Work out double swapped mass
                v_D0_doubleSwap = v_K_as_pi + v_pi_as_K;
                D0_M_doubleSwap = v_D0_doubleSwap.M();
                D0_deltaM_doubleSwap = std::abs(D0_M_doubleSwap - D0_M_PDG);

            }

            // Four body: swap D0 daughter kaon with both opposite-sign pions
            if (bodies == "fourBody") {

                // Work out which pion to use
                // D0K ID > 0: K+. Want to use pi-.
                // D0K ID < 0: K-. Want to use pi+.
                TLorentzVector * v_piToSwap;
                TLorentzVector * v_piNoSwap;
                if ((mode == "Kpipipi" && KstarK_ID < 0) || 
                        (mode == "piKpipi" && KstarK_ID > 0)) {
                    v_piToSwap = &v_D0PiPlus_P;
                    v_piNoSwap = &v_D0PiMinus_P;
                } else {
                    v_piToSwap = &v_D0PiMinus_P;
                    v_piNoSwap = &v_D0PiPlus_P;
                }

                // Distinguish high and low momentum pions
                TLorentzVector * v_D0Pi_low = (v_D0Pi_P.P() < v_piToSwap->P()) 
                    ? &v_D0Pi_P : v_piToSwap;
                TLorentzVector * v_D0Pi_high = (v_D0Pi_P.P() > v_piToSwap->P()) 
                    ? &v_D0Pi_P : v_piToSwap;

                // Work out vectors for pions with K mass
                v_low_pi_as_K.SetPxPyPzE(v_D0Pi_low->Px(), v_D0Pi_low->Py(), v_D0Pi_low->Pz(),
                        sqrt(pow(v_D0Pi_low->P(), 2) + pow(mass_K, 2)));
                v_high_pi_as_K.SetPxPyPzE(v_D0Pi_high->Px(), v_D0Pi_high->Py(), v_D0Pi_high->Pz(),
                        sqrt(pow(v_D0Pi_high->P(), 2) + pow(mass_K, 2)));

                // Work out double swapped masses
                v_D0_doubleSwap_low = v_K_as_pi + v_low_pi_as_K +
                    *v_D0Pi_high + *v_piNoSwap;
                v_D0_doubleSwap_high = v_K_as_pi + v_high_pi_as_K +
                    *v_D0Pi_low + *v_piNoSwap;
                D0_M_doubleSwap_low = v_D0_doubleSwap_low.M();
                D0_M_doubleSwap_high = v_D0_doubleSwap_high.M();
                D0_deltaM_doubleSwap_low = std::abs(D0_M_doubleSwap_low - D0_M_PDG);
                D0_deltaM_doubleSwap_high = std::abs(D0_M_doubleSwap_high - D0_M_PDG);
            }
        }

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
