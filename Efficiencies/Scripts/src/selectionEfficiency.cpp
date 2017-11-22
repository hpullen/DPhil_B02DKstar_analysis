#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <map>

#include "TFile.h"
#include "TTree.h"

// =======================================================================================
// Script to calculate selection efficiency for each mode/polarity/year, and print to file
// =======================================================================================
int main(int argc, char * arg[]) {

    // Values to loop through
    std::vector<std::string> modes = {"Kpi", "KK", "pipi", "Kpipipi", "pipipipi"};
    std::vector<std::string> years = {"2015", "2016"};
    std::vector<std::string> mags = {"up", "down"};

    // Map to hold results
    std::map<std::string, std::map<std::string, std::map<std::string, double>>> orig_events;

    // Open file with number of bookkeeping events
    std::ifstream bk_file("/home/pullen/analysis/B02DKstar/Efficiencies/Values/"
            "n_bookkeeping.txt");

    // Loop through and fill map of events
    std::string bk_mode;
    std::string bk_year;
    std::string bk_mag;
    std::string bk_events;
    bk_file >> bk_mode;
    while (!bk_file.eof()) {
        bk_file >> bk_year;
        bk_file >> bk_mag;
        bk_file >> bk_events;
        orig_events[bk_mode][bk_year][bk_mag] = std::stof(bk_events);
        bk_file >> bk_mode;
    }
    bk_file.close();

    // Open file to hold output
    std::ofstream outfile("/home/pullen/analysis/B02DKstar/Efficiencies/Values/"
            "selection_efficiency.txt");
    std::ofstream nfile("/home/pullen/analysis/B02DKstar/Efficiencies/Values/"
            "n_selected.txt");
    std::ofstream total_nfile("/home/pullen/analysis/B02DKstar/Efficiencies/Values/"
            "n_selected_total.txt");

    // Loop through modes
    for (auto mode : modes) {
        bool is_fourBody = (mode == "Kpipipi" || mode == "pipipipi");
        std::string bodies = ((is_fourBody) ? "fourBody" : "twoBody");

        int n_total = 0;

        // Loop through polarities
        std::string mc_path = "/data/lhcb/users/pullen/B02DKstar/MC/";
        for (auto mag : mags ) {


            // Loop through years
            for (auto year : years) {
                if (year == "2015" && is_fourBody) continue;

                // Open ROOT file with selected Monte Carlo
                TFile * file = TFile::Open((mc_path + bodies + "/" + mode + "/" +
                        year + "_" + mag + "/" + mode + "_selected.root").c_str(),
                        "READ");
                TTree * tree = (TTree*)file->Get("DecayTree");
                double nEntries = (double)tree->GetEntries();
                file->Close();

                // Divide entries by bookkeeping entries to get effiency
                double orig = orig_events[mode][year][mag];
                double eff = nEntries/orig;
                n_total += nEntries;

                // Calculate error
                double error = (1/orig) * sqrt(nEntries * (1 - nEntries / orig));

                // Write to file
                outfile << mode << " " << year << " " << mag << " " << eff << " " 
                    << error << std::endl;
                nfile << mode << " " << year << " " << mag << " " << nEntries
                    << std::endl;


            } // End year loop
        } // End polarity loop

        // Write total selected events for mode to file
        total_nfile << mode << " " << n_total << std::endl;

    } // End mode loop

    outfile.close();
    nfile.close();
    total_nfile.close();
    return 0;
}




