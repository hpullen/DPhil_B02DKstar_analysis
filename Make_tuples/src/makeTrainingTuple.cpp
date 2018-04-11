#include <iostream>
#include <string>
#include <cmath>

#include "TChain.h"
#include "TFile.h"
#include "TEventList.h"
#include "TTree.h"

#include "CutReader.hpp"

int main(int argc, char * argv[]) {

    // Check for correct number of args
    if (argc != 6 && argc != 7) {
        std::cout << "Usage: ./MakeTrainingTuple <data/MC> <2011/2012/2015/2016> "
            "<up/down> <twoBody/fourBody> <D0mode>" << std::endl;
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

    // Path to input file
    std::string inputPath = "/data/lhcb/users/pullen/B02DKstar/" + type + "/" +
        bodies + "/";
    if (type == "MC") {
        inputPath += mode + "/";
    }
    inputPath += year + "_" + mag + "/";
    std::string inputFile = inputPath + mode + ".root";
    std::cout << "Taking input from file " << inputFile << std::endl;

    // Read input file and tree
    TFile * file = TFile::Open(inputFile.c_str(), "READ");
    TTree * tree = (TTree*)file->Get("DecayTree");

    // Make output file and tree
    std::string outputFile = inputPath + mode + "_trainingSample.root";
    std::cout << "Output will be saved to " << outputFile << std::endl;
    TFile * new_file = TFile::Open(outputFile.c_str(), "RECREATE");
    TTree * new_tree = (TTree*)tree->CloneTree(0);

    // Get cut
    CutReader * cr = new CutReader();
    TCut cut = cr->GetCut("D0_M ");
    if (type == "data") {
        cut += "Bd_ConsD_M > 5800";
    } else {
        cut += "Bd_BKGCAT == 0";
    }

    // Draw to EventList
    tree->Draw(">>elist", cut);
    TEventList * elist = (TEventList*)gDirectory->Get("elist");

    // Fill new tree
    int passed = 0;
    for (int i = 0; i < elist->GetN(); i++) {
        int evt = elist->GetEntry(i);
        tree->GetEntry(evt);
        new_tree->Fill();
        passed++;
    }
    std::cout << "Filled new tree with " << passed << " entries.\n" << std::endl;

    // Save
    new_tree->Write();
    new_file->Close();
    delete tree;
    file->Close();
    delete file;
    delete cr;

    return 0;
}
