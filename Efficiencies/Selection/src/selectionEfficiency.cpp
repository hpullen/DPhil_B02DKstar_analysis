#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <map>

#include "TChain.h"

#include "CutReader.hpp"

// =======================================================================================
// Script to calculate selection efficiency for each mode/polarity/year, and print to file
// =======================================================================================
int main(int argc, char * argv[]) {

    // Get option
    enum Option {Signal, Bs, LowMass, RhoLowMass, weighted, weighted_Bs};
    Option opt;
    if (argc == 1) {
        opt = Option::Signal; 
    } else {
        std::string opt_str = argv[1];
        if (opt_str == "--signal") {
            opt = Option::Signal;
        } else if (opt_str == "--Bs") {
            opt = Option::Bs;
        } else if (opt_str == "--lowMass") {
            opt = Option::LowMass;
        } else if (opt_str == "--rhoLowMass") {
            opt = Option::RhoLowMass;
        } else {
            std::cout << "Unrecognised option " << opt_str << std::endl;
            return -1;
        }
    }

    // Map of categories and file locations
    std::map<std::string, std::string> cats;
    std::map<std::string, std::string> extra_files;
    std::map<std::string, std::string> preselection;
    std::map<std::string, std::string> extra_files_preselection;
    std::string mc_dir = "/data/lhcb/users/pullen/B02DKstar/MC/";
    std::vector<std::string> mags = {"up", "down"};
    for (auto mag : mags) {

        // Signal: add all available MC years for each mode
        if (opt == Option::Signal) {

            // Two-body signal MC
            for (std::string year : {"2011", "2012", "2015", "2016", "2017", "2018"}) {
            // for (std::string year : {"2011", "2012", "2015", "2016"}) {
                for (std::string mode : {"Kpi", "KK", "pipi"}) {
                    if (year == "2011" && mode != "Kpi") continue;
                    if (year == "2018" && mag == "down" && mode == "KK") continue;
                    cats[mode + "_" + year + "_" + mag] = 
                        mc_dir + "twoBody/" + mode + "/" + year + "_" + mag + "/"
                        + mode + "_selected.root";
                    preselection[mode + "_" + year + "_" + mag] = 
                        mc_dir + "twoBody/" + mode + "/" + year + "_" + mag + "/"
                        + mode + "_withWeights.root";
                }
            }

            // Four-body signal MC
            for (std::string year : {"2011", "2012", "2015", "2016", "2017", "2018"}) {
            // for (std::string year : {"2011", "2012", "2015", "2016"}) {
                for (std::string mode : {"Kpipipi", "pipipipi"}) {
                    if (mode == "pipipipi" && year == "2012") continue;
                    cats[mode + "_" + year + "_" + mag] = mc_dir + "fourBody/" + mode + 
                        "/" + year + "_" + mag + "/" + mode + "_selected.root";
                    preselection[mode + "_" + year + "_" + mag] = mc_dir + "fourBody/" + mode + 
                        "/" + year + "_" + mag + "/" + mode + "_withWeights.root";
                }
            }

        // Bs: Kpi only, add all years
        } else if (opt == Option::Bs) {
            for (std::string year : {"2011", "2012", "2015", "2016"}) {
                cats[year + "_" + mag] = mc_dir + "backgrounds/Bs/" + year + "_" + mag
                    + "/Kpi_selected.root";
                preselection[year + "_" + mag] = mc_dir + "backgrounds/Bs/" + year + "_" + mag
                    + "/Kpi_withWeights.root";
            }

        // Low mass: add gamma/pi and all helicities for 2012 and 2016
        } else if (opt == Option::LowMass || opt == Option::RhoLowMass) {
            std::vector<std::string> years = {"2016"};
            std::string dir = "rho_lowMass";
            if (opt == Option::LowMass) {
                years.push_back("2012");
                dir = "lowMass";
            }
            for (std::string year : years) {
                for (std::string particle : {"gamma", "pi"}) {
                    cats[particle + "_010_" + year + "_" + mag] =
                        mc_dir + "/backgrounds/" + dir + "/" + particle + "/010/"
                        + year + "_" + mag + "/Kpi_selected.root";
                    cats[particle + "_101_" + year + "_" + mag] =
                        mc_dir + "/backgrounds/" + dir + "/" + particle + "/100/"
                        + year + "_" + mag + "/Kpi_selected.root";
                    extra_files[particle + "_101_" + year + "_" + mag] =
                        mc_dir + "/backgrounds/" + dir + "/" + particle + "/001/"
                        + year + "_" + mag + "/Kpi_selected.root";
                    preselection[particle + "_010_" + year + "_" + mag] =
                        mc_dir + "/backgrounds/" + dir + "/" + particle + "/010/"
                        + year + "_" + mag + "/Kpi_withWeights.root";
                    preselection[particle + "_101_" + year + "_" + mag] =
                        mc_dir + "/backgrounds/" + dir + "/" + particle + "/100/"
                        + year + "_" + mag + "/Kpi_withWeights.root";
                    extra_files_preselection[particle + "_101_" + year + "_" + mag] =
                        mc_dir + "/backgrounds/" + dir + "/" + particle + "/001/"
                        + year + "_" + mag + "/Kpi_withWeights.root";
                }
            }
        }
    }

    // Map to hold results
    std::map<std::string, double> orig_events;

    // Open file with number of bookkeeping events
    std::string bk_name = "/home/pullen/analysis/B02DKstar/Efficiencies/Selection/Results/";
    switch (opt) {
        case Option::Signal : bk_name += "n_bookkeeping.txt"; break;
        case Option::weighted : bk_name += "n_bookkeeping.txt"; break;
        case Option::Bs : bk_name += "n_bookkeeping_Bs.txt"; break;
        case Option::weighted_Bs : bk_name += "n_bookkeeping_Bs.txt"; break;
        case Option::LowMass : bk_name += "n_bookkeeping_lowMass.txt"; break;
        case Option::RhoLowMass : bk_name += "n_bookkeeping_rho_lowMass.txt"; break;
    }
    std::cout << "Taking input from " << bk_name << std::endl;
    std::ifstream bk_file(bk_name);

    // Loop through and fill map of events
    std::string bk1;
    std::string bk2;
    std::string bk3;
    std::string bk4;
    std::string bk5;
    bk_file >> bk1;
    while (!bk_file.eof()) {

        // Read in category
        bk_file >> bk2;
        bk_file >> bk3;
        std::string cat = bk1 + "_" + bk2;     
        std::string events = bk3;
        if (opt != Option::Bs && opt != Option::weighted_Bs) {
            bk_file >> bk4;
            cat += "_" + bk3;
            events = bk4;
        }
        if (opt == Option::LowMass || opt == Option::RhoLowMass) {
            if (bk2 == "100" || bk2 == "001") bk2 = "101";
            bk_file >> bk5;
            cat = bk1 + "_" + bk2 + "_" + bk3 + "_" + bk4;
            events = bk5;
        }

        // Add events to map
        if (orig_events.count(cat) == 0) {
            orig_events[cat] = std::stod(events);
        } else {
            orig_events[cat] += std::stod(events);
        }
        bk_file >> bk1;
    }
    bk_file.close();

    // Open files to hold output
    std::string extra = "";
    if (opt == Option::Bs) extra = "_Bs";
    if (opt == Option::weighted) extra = "_weighted";
    if (opt == Option::weighted_Bs) extra = "_Bs_weighted";
    else if (opt == Option::LowMass) extra = "_lowMass";
    else if (opt == Option::RhoLowMass) extra = "_rho_lowMass";
    std::string outfile_name = "/home/pullen/analysis/B02DKstar/Efficiencies/"
        "Selection/Results/selection_efficiency" + extra + ".txt";
    std::string nfile_name = "/home/pullen/analysis/B02DKstar/Efficiencies/"
        "Selection/Results/n_selected" + extra + ".txt";
    std::ofstream outfile(outfile_name);
    std::ofstream nfile(nfile_name);

    // List of separate files to create
    std::map<std::string, std::ofstream*> sep_files;
    if (opt == Option::Signal) {
        for (std::string mode : {"Kpi", "KK", "pipi", "Kpipipi", "pipipipi"}) {
            sep_files[mode] = new std::ofstream("Results/Signal/" + mode + ".param");
        }
    } else if (opt == Option::LowMass) {
        for (std::string particle : {"gamma", "pi"}) {
            for (std::string hel : {"010", "101"}) {
                sep_files[particle + "_" + hel] = new std::ofstream("Results/LowMass/" + particle + "_" + hel + ".param");
            }
        }
    }

    // Loop through categories
    std::ofstream weight_file("weighted_vs_unweighted" + extra + ".txt");
    std::ofstream reco_file("Results/reco_efficiency" + extra + ".txt");
    std::ofstream offline_file("Results/offline_efficiency" + extra + ".txt");
    for (auto cat : cats) {
        if (cat.first == "KK_2018_down") continue;
        std::cout << "Processing category " << cat.first << std::endl;

        // Open ROOT file with preselection Monte Carlo
        TChain * pre_tree = new TChain("DecayTree");
        std::cout << "Opening unselected file " << preselection[cat.first]
            << std::endl;
        pre_tree->Add(preselection[cat.first].c_str());
        if (extra_files_preselection.count(cat.first) != 0) {
            pre_tree->Add(extra_files_preselection[cat.first].c_str());
        }
        
        // Open ROOT file with selected Monte Carlo
        TChain * tree = new TChain("DecayTree");
        std::cout << "Opening file " << cat.second << std::endl;
        tree->Add(cat.second.c_str());
        if (extra_files.count(cat.first) != 0) {
            tree->Add(extra_files[cat.first].c_str());
        }

        // Get entries
        double nEntries = (double)tree->GetEntries();
        double orig = orig_events[cat.first];
        double error = (1/orig) * sqrt(nEntries * (1 - nEntries / orig));

        // Get reconstrucion/stripping efficiency
        double nEntries_pre = (double)pre_tree->GetEntries();
        double pre_eff = nEntries_pre/orig;
        double pre_eff_err = (1/orig) * sqrt(nEntries_pre * (1 - nEntries_pre / orig));
        reco_file << cat.first << " " << pre_eff << " " << pre_eff_err << std::endl;

        // Get weighted offline selection efficiency
        // Weighted events before selection
        double sum_pre = 0; 
        double weight;
        pre_tree->SetBranchAddress("weight", &weight);
        for (int i = 0; i < pre_tree->GetEntries(); i++) {
            pre_tree->GetEntry(i);
            sum_pre += weight;
        }

        // Events in each category after selection, weighted
        double sum_post = 0;
        double weight_post;
        tree->SetBranchAddress("weight", &weight_post);
        for (int i = 0; i < tree->GetEntries(); i++) {
            tree->GetEntry(i);
            sum_post += weight_post;
        }
        double post_eff = sum_post/sum_pre;
        double post_eff_err = (1/nEntries_pre) * sqrt(nEntries * (1 - nEntries / nEntries_pre));
        double eff = sum_post/orig;
        double unweighted_eff = nEntries/orig;
        offline_file << cat.first << " " << post_eff << " " << post_eff_err << std::endl;

        // Write to file
        outfile << std::fixed << cat.first << " " << eff << " " << error << std::endl;
        nfile << cat.first << " " << nEntries << std::endl;

        // Check if we should write to separate file
        for (auto file : sep_files) {
            if (cat.first.find(file.first + "_") == 0) {
                *file.second << std::fixed << cat.first.substr(file.first.length() + 1, 
                        std::string::npos) << " " << eff << " " << error << std::endl;
            }
        }

        // Print weighted vs unweighted
        weight_file << cat.first << " " << unweighted_eff << " " << eff <<
            " " << eff - unweighted_eff << " " << error << std::endl;

        // Delete trees
        delete pre_tree;
        delete tree;


    } // End mode loop

    outfile.close();
    nfile.close();
    weight_file.close();
    reco_file.close();
    offline_file.close();
    for (auto file : sep_files) file.second->close();
    return 0;
}




