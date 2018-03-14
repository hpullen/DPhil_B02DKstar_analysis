// =============================================
// Script to plot data fit from a histogram file
// =============================================
#include <iostream>
#include "TFile.h"
#include "Plotter.hpp"


int main(int argc, char * argv[]) {

    // Get filename
    if (argc < 3) {
        std::cout << "./PlotFit <histogram-file> <plot-name> (<blind:Y/n>)" << std::endl;
        return -1;
    }
    std::string hist_file = argv[1];
    std::string plot_file = argv[2];
    bool blind = true;
    if (argc > 3) {
        if (std::string(argv[3]) == "N" || std::string(argv[3]) == "n") {
            blind = false;
        }
    }

    // Check if fit is split or not
    std::vector<std::string> raw_modes = {"Kpi", "piK", "KK", "pipi", "Kpipipi",
        "piKpipi", "pipipipi"};
    // std::vector<std::string> raw_modes = {"Kpi", "piK", "KK", "pipi"};
    TFile * file = TFile::Open(hist_file.c_str(), "READ");
    bool split = false;
    for (auto mode : raw_modes) {
        if (file->GetListOfKeys()->Contains(("fit_" + mode + "_plus").c_str())) split = true;
    }
    file->Close();

    // Plot
    std::vector<std::string> modes_to_plot;
    if (split) {
        for (auto mode : raw_modes) {
            modes_to_plot.push_back(mode + "_plus");
            modes_to_plot.push_back(mode + "_minus");
        }
    } else {
        modes_to_plot = raw_modes;
    }
    Plotter * plotter = new Plotter(hist_file, plot_file, modes_to_plot);

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

        // Determine shape type
        bool is_four_body = (mode_short == "Kpipipi" || 
                mode_short == "piKpipi" || mode_short == "pipipipi");
        bool is_favoured = (mode_short == "Kpi" || mode_short == "Kpipipi");
        std::string type = is_four_body ? "4body_" : "";

        // Add DKpipi to favoured mode
        if (is_favoured || mode_short == "piK") {
            plotter->AddComponent(mode, type + "DKpipi", DrawStyle::Filled,
                    kMagenta, "B^{+}#rightarrowDK^{+}#pi^{-}#pi^{+}");
        }

        // Add Bs components
        if (!is_favoured) {
            plotter->AddComponent(mode, type + "Bs", DrawStyle::Line, 
                    ANAGreen, "#bar{B}^{0}_{s}#rightarrowDK^{*0}");
            plotter->AddComponent(mode, type + "Bs_low", DrawStyle::Filled, 
                    kOrange + 7, "#bar{B}^{0}_{s}#rightarrowD^{*}K^{*0}");
        }

        // Add signal peak
        if (is_favoured || !blind) {
            if (plotter->IsInFile("signal_" + mode)) {
                plotter->AddComponent(mode, type + "signal", DrawStyle::Line, kRed + 2,
                        "B^{0}_{d}#rightarrowDK^{*0}");
            }
        }

        // Add other backgrounds
        plotter->AddComponent(mode, type + "rho", DrawStyle::Filled, 
                ANAPurple, "B^{0}#rightarrowD#rho^{0}");
    }

    // Add low mass background
    plotter->AddComponent("low", DrawStyle::Filled, kOrange,
            "B^{0}_{d}#rightarrowD^{*}K^{*0}");

    // Draw the plots
    plotter->Draw();
    delete plotter;
    return 0;

}
