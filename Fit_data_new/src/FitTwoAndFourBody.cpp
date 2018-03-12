#include <iostream>
#include <vector>

#include "TwoAndFourBodyFitter.hpp"
#include "Plotter.hpp"

using namespace Data;

int main(int argc, char * argv[]) {

    // Get option for splitting into B0 and B0bar
    bool split = false;
    if (argc > 1) split = (std::string(argv[1]) == "Y");
    if (split) std::cout << "Splitting fit into B0 and B0 bar" << std::endl;

    // Make the fitter
    TwoAndFourBodyFitter * fitter = new TwoAndFourBodyFitter(split);

    // Years and modes
    std::vector<std::string> years = {"2011", "2012", "2015", "2016"};
    std::vector<Mode> modes_twoBody = {Mode::Kpi, Mode::piK, Mode::KK, Mode::pipi};
    std::vector<Mode> modes_fourBody = {Mode::Kpipipi, Mode::piKpipi, Mode::pipipipi};

    // Add two body files
    std::string data_path = "/data/lhcb/users/pullen/B02DKstar/data/twoBody/";
    for (auto mode : modes_twoBody) {
        for (auto year : years) {
            std::string filepath_down = data_path + year + "_down/" +
                GetModeString(mode) + "_selected.root";
            std::string filepath_up = data_path + year + "_up/" +
                GetModeString(mode) + "_selected.root";
            fitter->AddFile(mode, filepath_down);
            fitter->AddFile(mode, filepath_up);
        }
    }

    // Add four body files
    std::string data_path_fourBody = "/data/lhcb/users/pullen/B02DKstar/data/fourBody/";
    for (auto mode : modes_fourBody) {
        for (auto year : years) {
            if (mode == Mode::pipipipi && (year == "2011" || year == "2012")) continue;
            std::string filepath_down = data_path_fourBody + year + "_down/" +
                GetModeString(mode) + "_selected.root";
            std::string filepath_up = data_path_fourBody + year + "_up/" +
                GetModeString(mode) + "_selected.root";
            fitter->AddFile(mode, filepath_down);
            fitter->AddFile(mode, filepath_up);
        }
    }

    // Add arguments: BDT cuts
    fitter->AddArg(Mode::Kpi, "BDTG_Kpi_run2", 0.5, 1);
    fitter->AddArg(Mode::piK, "BDTG_Kpi_run2", 0.5, 1);
    fitter->AddArg(Mode::KK, "BDTG_KK_run2", 0.5, 1);
    fitter->AddArg(Mode::pipi, "BDTG_pipi_run2", 0.5, 1);
    fitter->AddArg(Mode::Kpipipi, "BDTG_Kpipipi_run2", 0.5, 1);
    fitter->AddArg(Mode::piKpipi, "BDTG_Kpipipi_run2", 0.5, 1);
    fitter->AddArg(Mode::pipipipi, "BDTG_pipipipi_run2", 0.5, 1);

    // Filenames
    std::string results_file = split ? "Results/twoAndFourBody_data_split.root" :
        "Results/twoAndFourBody_data.root";
    std::string hist_file = split ? "Histograms/twoAndFourBody_data_split.root" :
        "Histograms/twoAndFourBody_data.root";
    std::string plot_file = split ? "Plots/twoAndFourBody_data_split" :
        "Plots/twoAndFourBody_data";

    // Fit
    fitter->PerformFit(results_file, hist_file);

    // Plot
    std::vector<std::string> modes_to_plot;
    std::vector<std::string> raw_modes = {"Kpi", "piK", "KK", "pipi", "Kpipipi",
        "piKpipi", "pipipipi"};
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

        // Add signal and DKpipi to favoured mode
        if (is_favoured) {
            plotter->AddComponent(mode, type + "signal", DrawStyle::Line, kRed + 2,
                    "B^{0}_{d}#rightarrowDK^{*0}");
        }
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

        // Add other backgrounds
        plotter->AddComponent(mode, type + "rho", DrawStyle::Filled, 
                ANAPurple, "B^{0}#rightarrowD#rho^{0}");
    }

    // Add low mass background
    plotter->AddComponent("low", DrawStyle::Filled, kOrange,
            "B^{0}_{d}#rightarrowD^{*}K^{*0}");
    // Draw the plots
    plotter->Draw();
    // delete plotter;

    return 0;
}
