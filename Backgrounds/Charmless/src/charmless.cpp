#include <iostream>
#include <fstream>

#include "TCanvas.h"
#include "TFile.h"
#include "TChain.h"
#include "TH1F.h"

#include "CutReader.hpp"

#include "FitDmass.hpp"
#include "FitBmass.hpp"

// =====================================================
// Script to estimate the amount of charmless background
// =====================================================
int main(int argc, char * argv[]) {

    // Get input args
    bool high_stats = false;
    if (argc != 3) {
        if (argc == 4 && std::string(argv[3]) == "--high_stats") {
            high_stats = true; 
        } else {
            std::cout << "Usage: ./Charmless <mode> <run=1/2/both>" << std::endl;
            return -1;
        }
    }
    std::string mode = argv[1];
    std::string run = argv[2];

    // Open files
    TChain * tree_full = new TChain("DecayTree");
    setPlotStyle();
    std::string bod = (mode == "Kpipipi" || mode == "piKpipi" || mode == "pipipipi")
        ? "four" : "two";
    std::string dir = "/data/lhcb/users/pullen/B02DKstar/data/" + bod + "Body/";
    for (std::string mag : {"up", "down"}) {
        if (run == "1" || run == "both") {
            tree_full->Add((dir + "/2011_" + mag + "/" + mode + "_withBDTG.root").c_str());
            tree_full->Add((dir + "/2012_" + mag + "/" + mode + "_withBDTG.root").c_str());
        } if (run == "2" || run == "both") {
            tree_full->Add((dir + "/2015_" + mag + "/" + mode + "_withBDTG.root").c_str());
            tree_full->Add((dir + "/2016_" + mag + "/" + mode + "_withBDTG.root").c_str());
        }
    }

    // Reduce with selection cut (except FDS and D0 mass)
    TCut cut;
    CutReader * cr = new CutReader(mode);
    std::vector<std::string> to_ignore = {"D0_M ", "D0_FDS"};
    if (high_stats) to_ignore.push_back("BDTG");
    cut = cr->GetCutExcept(to_ignore);
    TFile * file_temp = TFile::Open("temp.root", "RECREATE");
    TTree * tree = tree_full->CopyTree(cut);

    // Fit D0 mass with and without FDS cut
    double window_vs_sideband_ratio = FitDmass(tree, mode, run, high_stats, false);    
    FitDmass(tree, mode, run, high_stats, true);    

    // Fit B0 mass
    std::map<int, std::map<std::string, std::pair<double, double>>> yields = 
        FitBmass(tree, mode, run, high_stats);

    // Save results to file
    std::string extra_dir = high_stats ? "/high_stats/" : "";
    std::ofstream file("Results/" + extra_dir + mode + "_run_" + run + ".txt");
    for (auto cut : yields) {
        for (auto mode : cut.second) {
            file << cut.first << " " << mode.first << " " << 
                mode.second.first * window_vs_sideband_ratio << " " <<
                mode.second.second * window_vs_sideband_ratio << std::endl;
        }
    }
    file.close();
    file_temp->Close();
}
