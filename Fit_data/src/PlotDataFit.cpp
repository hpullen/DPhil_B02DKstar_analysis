// =============================================
// Script to plot data fit from a histogram file
// =============================================
#include <iostream>
#include "TFile.h"
#include "Plotter.hpp"
#include "LHCbPlotter.hpp"


int main(int argc, char * argv[]) {

    // Get filename
    if (argc < 3) {
        std::cout << "./PlotFit <histogram-file> <plot-name> (<--unblind --paper>)" << std::endl;
        return -1;
    }
    std::string hist_file = argv[1];
    std::string plot_file = argv[2];
    bool blind = true;
    bool paper = false;
    if (argc > 3) {
        for (int i = 3; i < argc; i++) {
            std::string arg = argv[i];
            if (arg == "--unblind") {
                std::cout << "Will show B mass peak in suppressed modes (not blind)" << std::endl;
                blind = false;
            } else if (arg == "--paper") {
                std::cout << "Will plot with LHCb paper style" << std::endl;
                paper = true;
            } else {
                std::cout << "Unrecognised option " << arg << std::endl;
            }
        }
    }

    // Check if fit is split or not
    std::vector<std::string> raw_modes = {"Kpi", "piK", "KK", "pipi", "Kpipipi",
        "piKpipi", "pipipipi"};
    // std::vector<std::string> raw_modes = {"Kpi", "piK"};
    // std::vector<std::string> raw_modes = {"Kpi", "piK", "KK", "pipi"};
    TFile * file = TFile::Open(hist_file.c_str(), "READ");
    bool split = false;
    auto keys = file->GetListOfKeys();
    for (auto mode : raw_modes) {
        if (keys->Contains(("fit_" + mode + "_plus").c_str())) split = true;
        else if (keys->Contains(("fit_" + mode + "_run2_plus").c_str())) {
            split = true;
        }
    }
    file->Close();

    // Plot
    std::vector<std::string> modes_to_plot;
    raw_modes = {"Kpi", "piK", "KK", "pipi", "Kpipipi",
        "piKpipi"};
    for (std::string run : {"_run1", "_run2"}) {
        for (auto mode : raw_modes) {
            if (split) {
                modes_to_plot.push_back(mode + run + "_plus");
                modes_to_plot.push_back(mode + run + "_minus");
            } else {
                modes_to_plot.push_back(mode + run);
            }
        }
    }
    if (split) {
        modes_to_plot.push_back("pipipipi_run2_plus");
        modes_to_plot.push_back("pipipipi_run2_minus");
    } else {
        modes_to_plot.push_back("pipipipi_run2");
    }
    Plotter * plotter; 
    if (!paper) {
        plotter = new Plotter(hist_file, plot_file, modes_to_plot);
    } else {
        plotter = new LHCbPlotter(hist_file, plot_file, modes_to_plot);
    }

    // Add combinatorial  
    plotter->AddComponent("expo", DrawStyle::Filled, ANABlue, "Combinatorial");


    // Loop through modes and add components to plot
    for (auto mode : modes_to_plot) {

        // Get mode name without plus or minus
        std::string mode_short = mode;
        if (mode.find("_plus") != std::string::npos) {
            mode_short = mode.substr(0, mode.find("_plus"));
        } else if (mode.find("_minus") != std::string::npos) {
            mode_short = mode.substr(0, mode.find("_minus"));
        }
        if (mode_short.find("_run") != std::string::npos) {
            mode_short = mode_short.substr(0, mode_short.find("_run"));
        }

        // Determine shape type
        bool is_four_body = (mode_short == "Kpipipi" || 
                mode_short == "piKpipi" || mode_short == "pipipipi");
        bool is_favoured = (mode_short == "Kpi" || mode_short == "Kpipipi");
        std::string type = is_four_body ? "4body_" : "";

        // Determine run
        std::string run = "";
        if (mode.find("_run1") != std::string::npos) run = "_run1";
        else if (mode.find("_run2") != std::string::npos) run = "_run2";

        // Add signal and backgrounds
        if (!blind) {
            plotter->AddComponent(mode, type + "signal", DrawStyle::Line, kRed + 2);
                    
        }
        plotter->AddComponent(mode, type + "DKpipi", DrawStyle::Filled, kCyan + 2);
        plotter->AddComponent(mode, type + "Bs", DrawStyle::Line, ANAGreen); 
        plotter->AddComponent(mode, type + "Bs_low" + run, DrawStyle::Filled, kOrange + 7);
        plotter->AddComponent(mode, type + "rho", DrawStyle::Filled, ANAPurple);
    }

    // Add low mass background
    plotter->AddComponent("low", DrawStyle::Filled, kOrange); 

    // Draw the plots
    plotter->Draw();
    return 0;

}
