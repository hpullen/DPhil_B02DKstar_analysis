#include <iostream>
#include <string>

#include "TTree.h"
#include "TFile.h"
#include "TCut.h"

// ====================================================
// Script to split an nTuple into B0 and B0-bar samples
// ====================================================
int main(int argc, char * argv[]) {

    // Make output name
    if (argc != 2) {
        std::cout << "Usage: ./SplitTuple <filename>" << std::endl;
        return -1;
    }
    std::string filename_in = argv[1];
    std::string filename_root = filename_in.substr(0, filename_in.find(".root"));
    std::string B0_outname = filename_root + "_B0.root";
    std::string B0bar_outname = filename_root + "_B0bar.root";
    std::cout << "Output will be saved to " << B0_outname << " and " << 
        B0bar_outname << std::endl;

    // Open input file
    TFile * file_in = TFile::Open(filename_in.c_str(), "READ");
    TTree * tree_in = (TTree*)file_in->Get("DecayTree");

    // Make cut strings
    std::string id;
    if (tree_in->GetListOfBranches()->Contains("KstarK_TRUEID")) {
        id = "KstarK_TRUEID";
    } else {
        id = "KstarK_ID";
    }
    TCut B0_cut = (id + " > 0").c_str();
    TCut B0bar_cut = (id + " < 0").c_str();

    // Copy tree: B0
    TFile * B0_out = TFile::Open(B0_outname.c_str(), "RECREATE");
    B0_out->cd();
    TTree * B0_tree = tree_in->CopyTree(B0_cut);
    B0_out->cd();
    B0_tree->Write("DecayTree");
    B0_out->Close();

    // Copy tree: B0-bar
    TFile * B0bar_out = TFile::Open(B0bar_outname.c_str(), "RECREATE");
    B0bar_out->cd();
    TTree * B0bar_tree = tree_in->CopyTree(B0bar_cut);
    B0bar_out->cd();
    B0bar_tree->Write("DecayTree");
    B0bar_out->Close();

    file_in->Close();
}
