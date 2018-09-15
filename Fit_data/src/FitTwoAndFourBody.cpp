#include <iostream>
#include <vector>

#include "TwoAndFourBodyFitter.hpp"
#include "Plotter.hpp"

using namespace Data;


// Function to convert between B and Bbar legends
std::string make_legend(std::string leg, std::string mode) {

    // // See if plus or minus
    // bool is_plus = false;
    // bool is_minus = false;
    // if (mode.find("plus") != std::string::npos) is_plus = true;
    // else if (mode.find("minus") != std::string::npos) is_minus = true;
    // else return leg;

    // if (is_plus) {
        // return leg;
    // }

    // if (is_minus) {

        // // Convert B to Bbar
        // std::string comp = "B^{0}";
        // std::size_t B0_pos = leg.find(comp);
        // if (B0_pos != std::string::npos) {
            // std::string end = leg.substr(comp.length(), std::string::npos);
            // leg = "#bar{B}^{0}" + end;
        // }

        // // Convert K*0 to K*0bar
        // comp = "K^{*0}";
        // std::size_t K_pos = leg.find(comp);
        // if (K_pos != std::string::npos) {
            // std::string pre = leg.substr(K_pos);
            // std::string end = leg.substr(K_pos + comp.length(), std::string::npos);
            // leg = pre + "#bar{K}^{*0}" + end;
        // }

        // return leg;
    // }
    return leg;
}


int main(int argc, char * argv[]) {

    // Get option for splitting into B0 and B0bar
    bool split = false;
    bool use_run1 = true;
    bool use_run2 = true;
    bool single_year = false;
    bool combine_runs = false;
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
        if (arg == "--combineRuns") {
            combine_runs = true;
            std::cout << "Fitting Run 1 and Run 2 as a single category" << std::endl;
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

    // Get run option
    Data::Run run_opt = Data::Run::Both;
    if (use_run1 && !use_run2) run_opt = Data::Run::Run1;
    else if (use_run1 && !use_run1) run_opt = Data::Run::Run2;
    else if (combine_runs) run_opt = Data::Run::BothCombined;

    // Make the fitter
    TwoAndFourBodyFitter * fitter = new TwoAndFourBodyFitter(split, run_opt);

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
    // std::vector<Mode> modes_fourBody = {Mode::Kpipipi, Mode::piKpipi};
    // std::vector<Mode> modes_fourBody = {Mode::Kpipipi, Mode::piKpipi};
    // std::vector<Mode> modes_twoBody = {Mode::Kpi, Mode::piK};
    std::vector<Mode> modes_fourBody = {Mode::Kpipipi, Mode::piKpipi};
    if (use_run2) modes_fourBody.push_back(Mode::pipipipi);

    // Add two body files
    std::string data_path = "/data/lhcb/users/pullen/B02DKstar/data/twoBody/";
    for (auto mode : modes_twoBody) {
        if (run_opt != Data::Run::Both) {
            for (auto year : years) {
                std::string filepath_down = data_path + year + "_down/" +
                    GetModeString(mode) + "_selected.root";
                std::string filepath_up = data_path + year + "_up/" +
                    GetModeString(mode) + "_selected.root";
                fitter->AddFile(mode, filepath_down);
                fitter->AddFile(mode, filepath_up);
            }
        } else {
            for (std::string mag : {"up", "down"}) {
                for (std::string year : {"2011", "2012"}) {
                    fitter->AddFile(mode, 1, data_path + year + "_" + mag + "/"
                            + GetModeString(mode) + "_selected.root");
                }
                for (std::string year : {"2015", "2016"}) {
                    fitter->AddFile(mode, 2, data_path + year + "_" + mag + "/"
                            + GetModeString(mode) + "_selected.root");
                }
            }
        }
    }

    // Add four body files
    std::string data_path_fourBody = "/data/lhcb/users/pullen/B02DKstar/data/fourBody/";
    for (auto mode : modes_fourBody) {
        if (run_opt != Data::Run::Both) {
            for (auto year : years) {
                if (mode == Mode::pipipipi && (year == "2011" || year == "2012")) continue;
                std::string filepath_down = data_path_fourBody + year + "_down/" +
                    GetModeString(mode) + "_selected.root";
                std::string filepath_up = data_path_fourBody + year + "_up/" +
                    GetModeString(mode) + "_selected.root";
                fitter->AddFile(mode, filepath_down);
                fitter->AddFile(mode, filepath_up);
            }
        } else {
            for (std::string mag : {"up", "down"}) {
                for (std::string year : {"2011", "2012"}) {
                    if (mode != Mode::pipipipi) {
                        fitter->AddFile(mode, 1, data_path_fourBody + year + "_" + mag + "/"
                                + GetModeString(mode) + "_selected.root");
                    }
                }
                for (std::string year : {"2015", "2016"}) {
                    fitter->AddFile(mode, 2, data_path_fourBody + year + "_" + mag + "/"
                            + GetModeString(mode) + "_selected.root");
                }
            }
        }
    }

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

    // Vector of runs
    std::vector<std::string> runs;
    if (run_opt == Data::Run::Both) {
        runs.push_back("_run1");
        runs.push_back("_run2");
    } else {
        runs.push_back("");
    }

    // Plot
    std::vector<std::string> modes_to_plot;
    // std::vector<std::string> raw_modes = {"Kpi"};
    std::vector<std::string> raw_modes = {"Kpi", "piK", "KK", "pipi", "Kpipipi",
        "piKpipi"};
    // // std::vector<std::string> raw_modes = {"Kpi", "piK", "KK", "pipi"};
    // if (use_run2 && !use_run1) raw_modes.push_back("pipipipi");
    // std::vector<std::string> raw_modes = {"Kpi", "piK", "Kpipipi", "piKpipi"};
    // std::vector<std::string> raw_modes = {"Kpi", "piK", "Kpipipi", "piKpipi",
        // "KK", "pipi"};
    // std::vector<std::string> raw_modes = {"Kpi", "piK"};
    for (auto run : runs) {
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

        // Add signal and DKpipi to favoured mode
        if (is_favoured) {
            plotter->AddComponent(mode, type + "signal", DrawStyle::Line, kRed + 2,
                    make_legend("B^{0}_{d}#rightarrowDK^{*0}", mode));
        }
        plotter->AddComponent(mode, type + "DKpipi", DrawStyle::Filled,
                kCyan + 2, make_legend("B^{+}#rightarrowDK^{+}#pi^{-}#pi^{+}", mode));

        // Add Bs components
        if (!is_favoured) {
            plotter->AddComponent(mode, type + "Bs", DrawStyle::Line, 
                    ANAGreen, "#bar{B}^{0}_{s}#rightarrowDK^{*0}");
            plotter->AddComponent(mode, type + "Bs_low" + run, DrawStyle::Filled, 
                    kOrange + 7, make_legend("#bar{B}^{0}_{s}#rightarrowD^{*}K^{*0}",
                        mode));
        }

        // Add other backgrounds
        plotter->AddComponent(mode, type + "rho", DrawStyle::Filled, 
                ANAPurple, make_legend("B^{0}#rightarrowD#rho^{0}", mode));
    }

    // Add low mass background
    plotter->AddComponent("low", DrawStyle::Filled, kOrange,
            "B^{0}_{d}#rightarrowD^{*}K^{*0}");

    // Draw the plots
    plotter->Draw();
    // delete plotter;

    return 0;
}
