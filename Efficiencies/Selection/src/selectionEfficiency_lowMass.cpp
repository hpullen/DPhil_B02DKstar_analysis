
#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <map>

#include "TFile.h"
#include "TTree.h"

// ================================================================
// Script to calculate selection efficiency for low mass background
// ================================================================
int main(int argc, char * arg[]) {
    
    // Values to loop through
    std::vector<std::string> mags = {"up", "down"};
    std::vector<std::string> particles = {"pi", "gamma"};
    std::vector<std::string> helicities = {"001", "010", "100"};
    std::vector<std::string> years = {"2012", "2016"};

    // Map for number of events in bookkeeping
    std::map<std::string, std::map<std::string, 
        std::map<std::string, std::map<std::string, double>>>> orig_events;

    // Look through file with event numbers
    std::ifstream bk_file("/home/pullen/analysis/B02DKstar/Efficiencies/Selection/"
            "Results/n_bookkeeping_lowMass.txt");
    std::string bk_particle;
    std::string bk_helicity;
    std::string bk_year;
    std::string bk_mag;
    std::string bk_events;
    bk_file >> bk_particle;
    while(!bk_file.eof()) {
        bk_file >> bk_helicity;
        bk_file >> bk_year;
        bk_file >> bk_mag;
        bk_file >> bk_events;
        orig_events[bk_particle][bk_helicity][bk_year][bk_mag] = std::stof(bk_events);
        bk_file >> bk_particle;
    }
    bk_file.close();

    // Open files for output
    std::ofstream outfile("/home/pullen/analysis/B02DKstar/Efficiencies/Selection/"
            "Results/selection_efficiency_lowMass.txt");
    std::ofstream nfile("/home/pullen/analysis/B02DKstar/Efficiencies/Selection/"
            "Results/n_selected_lowMass.txt");
    std::ofstream total_nfile("/home/pullen/analysis/B02DKstar/Efficiencies/Selection/"
            "Results/n_selected_total_lowMass.txt");

    // Loop through particles
    std::string mc_path = "/data/lhcb/users/pullen/B02DKstar/MC/backgrounds/lowMass/";
    for (auto particle : particles) {
        int n_tot_010 = 0;
        int n_tot_101 = 0;
        // Loop through helicities
        for (auto hel : helicities) {
            // Loop through polarities and years
            for (auto year : years) {
                for (auto mag : mags) {

                    // Open ROOT file
                    TFile * file = TFile::Open((mc_path + particle + "/" + hel +
                                "/" + year + "_" + mag + "/Kpi_selected.root").c_str(), "READ");
                    TTree * tree = (TTree*)file->Get("DecayTree");
                    double nEntries = (double)tree->GetEntries();
                    file->Close();

                    // Get efficiency
                    double orig = orig_events[particle][hel][year][mag];
                    double eff = nEntries/orig;
                    if (hel == "010") n_tot_010 += nEntries;
                    else n_tot_101 += nEntries;

                    // Calculate error
                    double error = (1/orig) * sqrt(nEntries * (1 - nEntries / orig));

                    // Write to file
                    outfile << particle << "_" << hel << "_" << year << "_" << mag << " "
                        << eff << " " << error << std::endl;
                    nfile << particle << "_" << hel << "_" << year << "_" << mag << " "
                        << nEntries << std::endl;

                    } // End polarity loop
                } // End year loop
            } // End helicity loop

        // Write total selected events for the particle to file
        total_nfile << particle << "_010 " << n_tot_010 << std::endl;
        total_nfile << particle << "_101 " << n_tot_101 << std::endl;
    } // End particle loop

    outfile.close();
    nfile.close();
    total_nfile.close();
    return 0;
}
