#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <map>

#include "TFile.h"
#include "TTree.h"

// ===========================================================
// Script to calculate selection efficiency for Bs background
// ===========================================================
int main(int argc, char * arg[]) {
    
    // Values to loop through
    std::vector<std::string> mags = {"up", "down"};
    std::vector<std::string> years = {"2011", "2012", "2015", "2016"};

    // Map for number of events in bookkeeping
    std::map<std::string, std::map<std::string, double>> orig_events;

    // Look through file with event numbers
    std::ifstream bk_file("/home/pullen/analysis/B02DKstar/Efficiencies/Selection/"
            "Results/n_bookkeeping_Bs.txt");
    std::string bk_year;
    std::string bk_mag;
    std::string bk_events;
    bk_file >> bk_year;
    while(!bk_file.eof()) {
        bk_file >> bk_mag;
        bk_file >> bk_events;
        orig_events[bk_year][bk_mag] = std::stof(bk_events);
        bk_file >> bk_year;
    }
    bk_file.close();

    // Open files for output
    std::ofstream outfile("/home/pullen/analysis/B02DKstar/Efficiencies/Selection/"
            "Results/selection_efficiency_Bs.txt");
    std::ofstream nfile("/home/pullen/analysis/B02DKstar/Efficiencies/Selection/"
            "Results/n_selected_Bs.txt");
    std::ofstream total_nfile("/home/pullen/analysis/B02DKstar/Efficiencies/Selection/"
            "Results/n_selected_total_Bs.txt");

    std::string mc_path = "/data/lhcb/users/pullen/B02DKstar/MC/backgrounds/Bs/";
    // Loop through polarities and years
    for (auto year : years) {
        for (auto mag : mags) {

            // Open ROOT file
            TFile * file = TFile::Open((mc_path + "/" + year + "_" + mag + 
                        "/Kpi_selected.root").c_str(), "READ");
            TTree * tree = (TTree*)file->Get("DecayTree");
            double nEntries = (double)tree->GetEntries();
            file->Close();

            // Get efficiency
            double orig = orig_events[year][mag];
            double eff = nEntries/orig;

            // Calculate error
            double error = (1/orig) * sqrt(nEntries * (1 - nEntries / orig));

            // Write to file
            outfile << year << "_" << mag << " " << eff << " " << error << std::endl;
            nfile << year << "_" << mag << " " << nEntries << std::endl;

        } // End year loop
    } // End helicity loop

    outfile.close();
    nfile.close();
    total_nfile.close();
    return 0;
}
