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
    TFile * file = TFile::Open(hist_file.c_str(), "READ");
    bool split = false;
    for (auto mode : raw_modes) {
        if (file->GetListOfKeys()->Contains((mode + "_plus").c_str())) split = true;
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

    // Add combinatorial and signal components
    plotter->AddComponent("expo", DrawStyle::Filled, ANABlue, "Combinatorial");
    if (split) {
        plotter->AddComponent("Kpi_plus", "signal", DrawStyle::Line, kRed + 2,
                "B^{0}_{d}#rightarrowDK^{*0}");
        plotter->AddComponent("Kpi_minus", "signal", DrawStyle::Line, kRed + 2,
                "#bar{B}^{0}_{d}#rightarrowD#bar{K}^{*0}");
        plotter->AddComponent("Kpipipi_plus", "4body_signal", DrawStyle::Line, kRed + 2,
                "B^{0}_{d}#rightarrowDK^{*0}");
        plotter->AddComponent("Kpipipi_minus", "4body_signal", DrawStyle::Line, kRed + 2,
                "#bar{B}^{0}_{d}#rightarrowD#bar{K}^{*0}");
        plotter->AddComponent("Kpi_plus", "DKpipi", DrawStyle::Filled, kMagenta,
                "B^{+}#rightarrowDK^{+}#pi^{-}#pi^{+}");
        plotter->AddComponent("Kpi_minus", "DKpipi", DrawStyle::Filled, kMagenta,
                "B^{-}#rightarrowDK^{-}#pi^{+}#pi^{-}");
    } else {
        plotter->AddComponent("Kpi", "signal", DrawStyle::Line, kRed + 2,
                "B^{0}_{d}#rightarrowDK^{*0}");
        plotter->AddComponent("Kpipipi", "4body_signal", DrawStyle::Line, kRed + 2,
                "B^{0}_{d}#rightarrowDK^{*0}");
        plotter->AddComponent("Kpi", "DKpipi", DrawStyle::Filled, kMagenta,
                "B^{+}#rightarrowDK^{+}#pi^{-}#pi^{+}");
    }

    // Add Bs components
    std::vector<std::string> modes_with_Bs = {"piK", "KK", "pipi", "piKpipi",
        "pipipipi"};
    for (auto mode : modes_with_Bs) {
        if (split) {
            plotter->AddComponent(mode + "_plus", "Bs_low", DrawStyle::Filled, kOrange + 7,
                    "#bar{B}^{0}_{s}#rightarrowD^{*}K^{*0}");
            plotter->AddComponent(mode + "_minus", "Bs_low", DrawStyle::Filled, kOrange + 7,
                    "B^{0}_{s}#rightarrowD^{*}#bar{K}^{*0}");
            if (mode == "piKpipi" || mode == "pipipipi") {
                plotter->AddComponent(mode + "_plus", "4body_Bs", DrawStyle::Line, ANAGreen,
                        "#bar{B}^{0}_{s}#rightarrowDK^{*0}");
                plotter->AddComponent(mode + "_minus", "4body_Bs", DrawStyle::Line, ANAGreen,
                        "B^{0}_{s}#rightarrowD#bar{K}^{*0}");
                if (!blind) {
                    plotter->AddComponent(mode + "_plus", "4body_signal", DrawStyle::Line, kRed + 2,
                            "B^{0}_{d}#rightarrowDK^{*0}");
                    plotter->AddComponent(mode + "_minus", "4body_signal", DrawStyle::Line, kRed + 2,
                            "#bar{B}^{0}_{d}#rightarrowD#bar{K}^{*0}");
                }
            } else {
                plotter->AddComponent(mode + "_plus", "Bs", DrawStyle::Line, ANAGreen,
                        "#bar{B}^{0}_{s}#rightarrowDK^{*0}");
                plotter->AddComponent(mode + "_minus", "Bs", DrawStyle::Line, ANAGreen,
                        "B^{0}_{s}#rightarrowD#bar{K}^{*0}");
                if (!blind) {
                    plotter->AddComponent(mode + "plus", "signal", DrawStyle::Line, kRed + 2,
                            "B^{0}_{d}#rightarrowDK^{*0}");
                    plotter->AddComponent(mode + "minus", "signal", DrawStyle::Line, kRed + 2,
                            "#bar{B}^{0}_{d}#rightarrowD#bar{K}^{*0}");
                }
            }
        } else {
            plotter->AddComponent(mode, "Bs_low", DrawStyle::Filled, kOrange + 7,
                    "#bar{B}^{0}_{s}#rightarrowD^{*}K^{*0}");
            if (mode == "piKpipi" || mode == "pipipipi") {
                plotter->AddComponent(mode, "4body_Bs", DrawStyle::Line, ANAGreen,
                        "#bar{B}^{0}_{s}#rightarrowDK^{*0}");
                if (!blind) {
                    plotter->AddComponent(mode, "4body_signal", DrawStyle::Line, kRed + 2,
                            "B^{0}_{d}#rightarrowDK^{*0}");
                }
            } else {
                plotter->AddComponent(mode, "Bs", DrawStyle::Line, ANAGreen,
                        "#bar{B}^{0}_{s}#rightarrowDK^{*0}");
                if (!blind) {
                    plotter->AddComponent(mode, "signal", DrawStyle::Line, kRed + 2,
                            "B^{0}_{d}#rightarrowDK^{*0}");
                }
            }
        }
    }

    // Add other backgrounds
    plotter->AddComponent("low", DrawStyle::Filled, kOrange,
            "B^{0}_{d}#rightarrowD^{*}K^{*0}");
    plotter->AddComponent("rho", DrawStyle::Filled, ANAPurple,
            "B^{0}#rightarrowD#rho^{0}");

    // Draw the plots
    plotter->Draw();
    delete plotter;
    return 0;

}
