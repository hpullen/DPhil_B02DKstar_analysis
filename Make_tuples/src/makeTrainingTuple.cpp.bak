#include <iostream>
#include <string>
#include <cmath>

#include "TChain.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TTree.h"

inline double Product(const TLorentzVector& p1, const TLorentzVector& p2,
        const TLorentzVector& pX) {
    return - p1.Dot(p2) + p1.Dot(pX) * p2.Dot(pX) / pX.Dot(pX);
}

void LorentzVectorBranch(TTree* tree, TLorentzVector& vec,
        const std::string& name) {
    tree->SetBranchAddress((name + "_PE").c_str(), &vec[3]);
    tree->SetBranchAddress((name + "_PX").c_str(), &vec[0]);
    tree->SetBranchAddress((name + "_PY").c_str(), &vec[1]);
    tree->SetBranchAddress((name + "_PZ").c_str(), &vec[2]);
}

int main(int argc, char * argv[]) {

    // Check for correct number of args
    if (argc != 6 && argc != 7) {
        std::cout << "Usage: ./MakeTrainingTuple <data/MC> <2011/2012/2015/2016> "
            "<up/down> <twoBody/fourBody> <D0mode> (<treename>)" << std::endl;
        return -1;
    }

    // File type (data vs. MC)
    std::string type = argv[1];

    // Year (2011, 2012, 2015, 2016)
    std::string year = argv[2];

    // Polarity (up vs. down)
    std::string mag = argv[3];

    // 2-body vs. 4-body
    std::string bodies = argv[4];

    // D0 decay mode
    std::string mode = argv[5];

    // Optional: treename
    std::string treename = "Tuple_" + mode;
    if (argc == 7) {
        treename = argv[6];
    }
    treename += "/DecayTree";

    // Path to input file
    std::string inputPath = "/data/lhcb/users/pullen/B02DKstar/" + type + "/" +
        bodies + "/";
    if (type == "MC") {
        inputPath += mode + "/";
    }
    inputPath += year + "_" + mag + "/";
    std::string inputFile = inputPath + "Total.root";
    std::cout << "Taking input from tree " << treename << " in file " <<
        inputFile << std::endl;

    // Read input file and tree
    TFile * file = TFile::Open(inputFile.c_str(), "READ");
    TTree * tree = (TTree*)file->Get(treename.c_str());

    // Make output file and tree
    std::string outputFile = inputPath + mode + "_trainingSample.root";
    std::cout << "Output will be saved to " << outputFile << std::endl;
    TFile * new_file = TFile::Open(outputFile.c_str(), "RECREATE");
    TTree * new_tree = (TTree*)tree->CloneTree(0);

    // Variables needed
    double Kstar_M;
    double D0_M;
    float Bd_ConsD_M;
    double helicityAngle;
    int bkgCat = 0;
    TLorentzVector KstarK_P, KstarPi_P, D0_P;

    // Constants
    const double D0_PDG_M = 1864.84;
    const double D0_range = 25.0;
    const double Kstar_PDG_M = 895.55;
    const double Kstar_range = 50.0;

    // Set branch addresses
    tree->SetBranchAddress("Kstar_M", &Kstar_M);
    tree->SetBranchAddress("D0_M", &D0_M);
    tree->SetBranchAddress("Bd_ConsD_M", &Bd_ConsD_M);
    if (type == "MC") {
        tree->SetBranchAddress("Bd_BKGCAT", &bkgCat);
    }

    // Make TLorentzVector branches
    LorentzVectorBranch(tree, KstarK_P, "KstarK");
    LorentzVectorBranch(tree, KstarPi_P, "KstarPi");
    LorentzVectorBranch(tree, D0_P, "D0");

    // Make new branch for helicity angle
    new_tree->Branch("Kstar_helicity_angle", &helicityAngle, 
            "Kstar_helicity_angle/D");

    // Loop over tree entries
    int n_entries = tree->GetEntries();
    std::cout << "Looping over " << n_entries << " entries." << std::endl;
    int n_pass = 0; 
    for (int i = 0; i < n_entries; i++) {
        tree->GetEntry(i);
        if (i % 100000 == 0) {
            std::cout << "Processing event " << i << std::endl;
        }

        // Work out helicity angle
        helicityAngle = Product(KstarK_P, D0_P, KstarK_P + KstarPi_P) / 
            sqrt(Product(KstarK_P, KstarK_P, KstarK_P + KstarPi_P) * 
                    Product(D0_P, D0_P, KstarK_P + KstarPi_P));

        // Fill tree if cuts are passed
        if (type == "MC") {
            if (std::abs(Kstar_M - Kstar_PDG_M) < Kstar_range 
                    && std::abs(D0_M - D0_PDG_M) < D0_range
                    && std::abs(helicityAngle) > 0.4
                    && bkgCat == 0) {
                new_tree->Fill();
                n_pass++;
            }
        } else if (type == "data") {
            if (std::abs(Kstar_M - Kstar_PDG_M) < Kstar_range 
                    && std::abs(D0_M - D0_PDG_M) < D0_range
                    && std::abs(helicityAngle) > 0.4
                    && Bd_ConsD_M > 5800) {
                new_tree->Fill();
                n_pass++;
            }
        }
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
