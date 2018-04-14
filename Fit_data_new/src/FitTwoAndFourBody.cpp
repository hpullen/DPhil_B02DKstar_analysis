#include <iostream>
#include <vector>

#include "TwoAndFourBodyFitter.hpp"
#include "Plotter.hpp"

using namespace Data;

int main(int argc, char * argv[]) {

    // Get option for splitting into B0 and B0bar
    bool split = false;
    bool use_run1 = true;
    bool use_run2 = true;
    bool single_year = false;
    std::string year_to_use;
    bool loose_cut = false;
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--split") {
            split = true;
            std::cout << "Splitting fit into B0 and B0 bar" << std::endl;
        } 
        if (arg == "--run1") {
            use_run2 = false;
            std::cout << "Fitting to Run 1 data only" << std::endl;
        } else if (arg == "--run2") {
            use_run1 = false;
            std::cout << "Fitting to Run 2 data only" << std::endl;
        }
        else if (arg == "--loose") {
            loose_cut = true;
            std::cout << "Performing fit with loose (> 0) BDT cut" << std::endl;
        }
        for (std::string year : {"2011", "2012", "2015", "2016"}) {
            if (arg == "--" + year) {
                single_year = true;
                year_to_use = year;
                if (year_to_use == "2011" || year_to_use == "2012") {
                    use_run1 = true;
                    use_run2 = false;
                } else {
                    use_run2 = true;
                    use_run1 = false;
                }
            }
        }
    }
    if (single_year) std::cout << "Fitting to " << year_to_use << " only." << 
        std::endl;

    // Make the fitter
    TwoAndFourBodyFitter * fitter = new TwoAndFourBodyFitter(split, use_run2);

    // Years and modes
    std::vector<std::string> years;
    if (single_year) {
        years.push_back(year_to_use);
    } else {
        if (use_run1) {
            years.push_back("2011");
            years.push_back("2012");
        } 
        if (use_run2) {
            years.push_back("2015");
            years.push_back("2016");
        } 
    }
    std::vector<Mode> modes_twoBody = {Mode::Kpi, Mode::piK, Mode::KK, Mode::pipi};
    std::vector<Mode> modes_fourBody = {Mode::Kpipipi, Mode::piKpipi};
    if (use_run2) modes_fourBody.push_back(Mode::pipipipi);

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
    double BDT_cut = (loose_cut) ? 0.0 : 0.5;
    fitter->AddArg(Mode::Kpi, "BDTG_Kpi_run2", BDT_cut, 1);
    fitter->AddArg(Mode::piK, "BDTG_Kpi_run2", BDT_cut, 1);
    fitter->AddArg(Mode::KK, "BDTG_KK_run2", BDT_cut, 1);
    fitter->AddArg(Mode::pipi, "BDTG_pipi_run2", BDT_cut, 1);
    fitter->AddArg(Mode::Kpipipi, "BDTG_Kpipipi_run2", BDT_cut, 1);
    fitter->AddArg(Mode::piKpipi, "BDTG_Kpipipi_run2", BDT_cut, 1);
    fitter->AddArg(Mode::pipipipi, "BDTG_pipipipi_run2", BDT_cut, 1);

    // Filenames
    std::string extra = "";
    if (single_year) extra = "_" + year_to_use;
    else if (!use_run1) extra = "_run2";
    else if (!use_run2) extra = "_run1";
    else if (loose_cut) extra = "_looseBDT";
    std::string results_file = split ? "Results/twoAndFourBody_data_split" + 
        extra + ".root" : "Results/twoAndFourBody_data" + extra + ".root";
    std::string hist_file = split ? "Histograms/twoAndFourBody_data_split" + extra 
        + ".root" : "Histograms/twoAndFourBody_data" + extra + ".root";
    std::string plot_file = split ? "Plots/twoAndFourBody_data_split" + extra :
        "Plots/twoAndFourBody_data" + extra;

    // Fit
    fitter->PerformFit(results_file, hist_file);

    // Plot
    std::vector<std::string> modes_to_plot;
    std::vector<std::string> raw_modes = {"Kpi", "piK", "KK", "pipi", "Kpipipi",
        "piKpipi"};
    if (use_run2) raw_modes.push_back("pipipipi");
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
        // if (is_favoured || mode_short == "piK") {
            // plotter->AddComponent(mode, type + "DKpipi", DrawStyle::Filled,
                    // kMagenta, "B^{+}#rightarrowDK^{+}#pi^{-}#pi^{+}");
        // }

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
