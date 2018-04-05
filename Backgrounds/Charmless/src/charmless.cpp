#include <iostream>
#include <fstream>

#include "TCanvas.h"
#include "TChain.h"
#include "TH1F.h"

#include "FitDmass.hpp"
#include "FitBmass.hpp"

// =====================================================
// Script to estimate the amount of charmless background
// =====================================================
int main(int argc, char * argv[]) {

    // Get input args
    if (argc != 3) {
        std::cout << "Usage: ./Charmless <mode> <run=1/2/both>" << std::endl;
        return -1;
    }
    std::string mode = argv[1];
    std::string run = argv[2];

    // Open files
    TChain * tree = new TChain("DecayTree");
    setPlotStyle();
    std::string bod = (mode == "Kpipipi" || mode == "piKpipi" || mode == "pipipipi")
        ? "four" : "two";
    std::string dir = "/data/lhcb/users/pullen/B02DKstar/data/" + bod + "Body/";
    for (std::string mag : {"up", "down"}) {
        if (run == "1" || run == "both") {
            tree->Add((dir + "/2011_" + mag + "/" + mode + 
                        "_selected_full_D0_mass.root").c_str());
            tree->Add((dir + "/2012_" + mag + "/" + mode + 
                        "_selected_full_D0_mass.root").c_str());
        } if (run == "2" || run == "both") {
            tree->Add((dir + "/2015_" + mag + "/" + mode + 
                        "_selected_full_D0_mass.root").c_str());
            tree->Add((dir + "/2016_" + mag + "/" + mode + 
                        "_selected_full_D0_mass.root").c_str());
        }
    }

    // Fit D0 mass
    double window_vs_sideband_ratio = FitDmass(tree, mode, run);    

    // Fit B0 mass
    std::map<int, std::map<std::string, std::pair<double, double>>> yields = 
        FitBmass(tree, mode, run);

    // Save results to file
    std::ofstream file("Results/" + mode + "_run_" + run + ".txt");
    for (auto cut : yields) {
        for (auto mode : cut.second) {
            file << cut.first << " " << mode.first << " " << 
                mode.second.first * window_vs_sideband_ratio << " " <<
                mode.second.second * window_vs_sideband_ratio << std::endl;
        }
    }
    file.close();
}
