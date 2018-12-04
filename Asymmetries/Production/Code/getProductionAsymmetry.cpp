#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

#include "TTree.h"
#include "TEventList.h"
#include "TFile.h"
#include "TString.h"

// =======================================================
// Script to calculate production asymmetry for an MC file
// =======================================================
struct asym_bin {
    int number;
    double min_pT;
    double max_pT;
    double min_y;
    double max_y;
    double asym;
    double err;
    double n_in_bin;
};


int main(int argc, char * argv[]) {

    // Input args
    if (argc != 4) {
        std::cout << "Usage: ./GetProductionAsymmetry <filename> <B0/Bs> <7/8>"
            << std::endl;
    }
    TString filename = argv[1];
    std::string particle = argv[2];
    std::string s = argv[3];

    // Store bins
    std::vector<asym_bin*> bins;

    // Read in asymmetries and bins
    std::string bin_filename = "../Tables/" + particle + "_" + s + "TeV.txt";
    std::ifstream binfile(bin_filename);
    std::string line;
    int num = 1;
    while (std::getline(binfile, line)) {
        asym_bin * current_bin = new asym_bin;
        std::stringstream ss(line);
        std::string min_pT, max_pT, min_y, max_y, asym, stat, sys;
        ss >> min_pT >> max_pT >> min_y >> max_y >> asym >> stat >> sys;
        current_bin->number = num;
        current_bin->min_pT = std::stod(min_pT);
        current_bin->max_pT = std::stod(max_pT);
        current_bin->min_y = std::stod(min_y);
        current_bin->max_y = std::stod(max_y);
        current_bin->asym = std::stod(asym);
        double stat_err = std::stod(stat);
        double sys_err = std::stod(sys);
        current_bin->err = sqrt(stat_err * stat_err  + sys_err * sys_err);
        current_bin->n_in_bin = 0;
        bins.push_back(current_bin);
        num++;
    }

    // Sum of asymmetries and total
    double count = 0;

    // Open file
    TFile * file = TFile::Open(filename, "READ");
    TTree * tree = (TTree*)file->Get("DecayTree");

    // Truth match
    std::string cut = (particle == "B0") ? "0" : "20";
    tree->Draw(">>elist", ("Bd_BKGCAT == " + cut).c_str());
    TEventList * evtList = (TEventList*)gDirectory->Get("elist");

    // Set branches
    double y;
    double PT;
    tree->SetBranchAddress("Bd_LOKI_Y", &y);
    tree->SetBranchAddress("Bd_PT", &PT);

    // Bin each event
    for (int i = 0 ; i < evtList->GetN(); i++) {
        int evt = evtList->GetEntry(i);
        tree->GetEntry(evt);
        for (auto bin : bins) {
            if (PT/1000 > bin->min_pT && PT/1000 < bin->max_pT) {
                if (y > bin->min_y && y < bin->max_y) {
                    bin->n_in_bin++;
                    count++;
                    break;
                }
            }
        }
    }
    file->Close();

    // Average over bins
    double sq_error_sum = 0;
    double asym_sum = 0;
    for (auto bin : bins) {
        if (bin->n_in_bin < 0.5) continue;

        // Add contribution from this bin
        double bin_asym_sum = bin->asym * bin->n_in_bin/count;
        asym_sum += bin_asym_sum;

        // Calculate uncertainties
        // Binomial uncertainty on proportion of events in bin
        // double bin_efficiency_error = 1/count * sqrt(bin->n_in_bin * (1 - bin->n_in_bin/count));
        // double frac_bin_efficiency_error = bin_efficiency_error/(bin->n_in_bin/count);

        // Fractional uncertainties on asymmetry in the bin
        // double frac_err = bin->err/bin->asym;

        // Add fractional errors in quadrature
        sq_error_sum += bin->err;
        // sq_error_sum += pow(bin_asym_sum, 2) *
            // (pow(frac_bin_efficiency_error, 2) + pow(frac_err, 2));
    }

    // Print results
    std::cout << "PRODUCTION ASYMMETRY: " << asym_sum <<  " +/- " << 
        sqrt(sq_error_sum);

    return 0;
}
