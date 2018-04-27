#include <iostream>
#include <string>

#include "TCut.h"
#include "TFile.h"
#include "TTree.h"

#include "CutReader.hpp"

// ======================================
// Script to apply selection to an nTuple
// ======================================


int main(int argc, char * argv[]) {

    // Check args
    if (argc != 4 && argc !=5) {
        std::cout << "Usage: ./MakeSelectedTuple <2011/2012/2015/2016> <up/down> " <<
            "<D0mode> (<--fullD/--Bmass>)" << std::endl;
        return -1;
    }

    // Check fifth arg
    bool full_D = false;
    bool B_mass = false;
    if (argc == 5) {
        if (std::string(argv[4]) == "--fullD") {
            full_D = true;
            std::cout << "Making an nTuple with full D0 mass and FDS range" 
                << std::endl;
        } else if (std::string(argv[4]) == "--Bmass") {
            B_mass = true;
            std::cout << "Making an nTuple with B0 mass window only"
                << std::endl;
        } else {
            std::cout << "Input arg \"" << argv[4] << "\" not recognised." <<
                std::endl;
            return -1;
        }
    }

    // Get year, mode, polarity
    std::string year = argv[1];
    std::string mag = argv[2];
    std::string mode = argv[3];
    std::string bod = (mode == "Kpipipi" || mode == "piKpipi" || mode == "pipipipi")
        ? "fourBody" : "twoBody";

    // Name of input tuple
    std::string infile_name = "/data/lhcb/users/pullen/B02DKstar/data/" + bod + "/"
        + year + "_" + mag + "/" + mode + "_withBDTG.root";

    // Read input file and tree
    TFile * infile = TFile::Open(infile_name.c_str(), "READ");
    TTree * intree = (TTree*)infile->Get("DecayTree");

    // Make output file
    std::string outfile_name = "/data/lhcb/users/pullen/B02DKstar/data/" + bod + "/"
        + year + "_" + mag + "/" + mode + "_selected";
    if (full_D) outfile_name += "_full_D0_mass";
    else if (B_mass) outfile_name += "_B_mass_window";
    outfile_name += ".root";
    TFile * outfile = TFile::Open(outfile_name.c_str(), "RECREATE");

    // Read in cut
    CutReader * cr = new CutReader(mode);

    // Copy tree with selection applied
    TCut cut;
    if (full_D) {
        std::vector<std::string> to_ignore = {"D0_M ", "D0_FDS"};
        cut = cr->GetCutExcept(to_ignore);
    } else {
        cut = cr->GetCut();
    }
    if (B_mass) {
        cut += "abs(Bd_ConsD_MD - 5279.81) < 25.0";
    }
    TTree * outtree = intree->CopyTree(cut);

    // Save
    outfile->cd();
    outtree->Write("DecayTree");
    outfile->Close();
    infile->Close();

}

