#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <map>

#include "TFile.h"
#include "TTree.h"

// =================================================================================
// Script to calculate PID efficiency for each mode/polarity/year, and print to file
// =================================================================================
int main(int argc, char * arg[]) {

    // Values to loop through
    //std::vector<std::string> modes = {"Kpi", "KK", "pipi", "Kpipipi", "pipipipi"};
    std::vector<std::string> modes = {"Kpi", "KK", "pipi"};
    std::vector<std::string> years = {"2015", "2016"};
    std::vector<std::string> mags = {"up", "down"};

    // Open file to hold output
    std::ofstream outfile("/home/pullen/analysis/B02DKstar/Efficiencies/Values/"
            "PID_efficiency.txt");

    // Loop through polarities
    std::string mc_path = "/data/lhcb/users/pullen/B02DKstar/MC/";
    for (auto mag : mags ) {

        // Loop through modes
        for (auto mode : modes) {
            bool is_fourBody = (mode == "Kpipipi" || mode == "pipipipi");
            std::string bodies = ((is_fourBody) ? "fourBody" : "twoBody");

            // Loop through years
            for (auto year : years) {
                if (year == "2015" && is_fourBody) continue;

                // Open ROOT file with MC with PID efficiency
                TFile * file = TFile::Open((mc_path + bodies + "/" + mode + "/" +
                        year + "_" + mag + "/" + mode + "_selected_withPIDeffs.root").c_str(),
                        "READ");
                TTree * tree = (TTree*)file->Get("CalibTool_PIDCalibTree");

                // Variables to hold sums
                double sum_eff = 0;
                double sum_error = 0;

                // Set up branch for error and efficiency
                Float_t eff;
                Float_t error;
                tree->SetBranchAddress("Event_PIDCalibEff", &eff);
                tree->SetBranchAddress("Event_PIDCalibErr", &error);

                // Loop through tree and calculate weighted mean of effiency
                for (int i = 0; i < tree->GetEntries(); i++) {
                    tree->GetEntry(i);
                    sum_eff += eff/pow(error, 2);
                    sum_error += 1/pow(error, 2);
                }

                // Calculate final error and efficiency
                double final_eff = sum_eff/sum_error;
                double final_err = sqrt(1/sum_error);

                // Write to file
                outfile << mode << " " << year << " " << mag << " " << final_eff << " " 
                    << final_err << std::endl;

                file->Close();

            } // End year loop
        } // End mode loop
    } // End polarity loop

    outfile.close();
    return 0;
}




