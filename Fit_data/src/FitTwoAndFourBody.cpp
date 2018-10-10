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
    bool combine_runs = false;
    std::string year_to_use;
    bool limited_modes = false;
    bool sep_R = false;
    std::vector<std::string> limited_modes_to_use;
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
        if (arg == "--modes") {
        limited_modes = true;
        for (int j = i + 1; j < argc; j++) {
            std::string jarg = argv[j];
            if (jarg.find("--") != std::string::npos) {
                i = j - 1;
                break;
                } else {
                    limited_modes_to_use.push_back(jarg);
                }
            }
        }
        if (arg == "--sep_R") {
            sep_R = true;
            std::cout << "Extracting separate R+/- for run 1 and run 2" << std::endl;
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
    TwoAndFourBodyFitter * fitter;
    if (!limited_modes) {
       fitter = new TwoAndFourBodyFitter(split, run_opt, {"Kpi", "piK", "KK", "pipi",
               "Kpipipi", "piKpipi", "pipipipi"});
    } else {
       fitter = new TwoAndFourBodyFitter(split, run_opt, limited_modes_to_use);
    }
    if (sep_R) fitter->SeparateRruns();

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
    std::vector<Mode> modes_twoBody;
    std::vector<Mode> modes_fourBody;
    if (!limited_modes) {
            modes_twoBody = {Mode::Kpi, Mode::piK, Mode::KK, Mode::pipi};
            modes_fourBody = {Mode::Kpipipi, Mode::piKpipi};
        if (use_run2) modes_fourBody.push_back(Mode::pipipipi);
    } else {
        std::cout << "Fit will be performed for modes: ";
        for (auto mode : limited_modes_to_use) {
            std::cout << mode << " ";
            if (mode == "Kpipipi" || mode == "piKpipi" || mode == "pipipipi") {
                modes_fourBody.push_back(GetModeFromString(mode));
            } else {
                modes_twoBody.push_back(GetModeFromString(mode));
            }
        }
        std::cout << std::endl;
    }

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
    if (sep_R) extra += "_sepR";
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
    std::vector<std::string> raw_modes;
    if (!limited_modes) {
        raw_modes = {"Kpi", "piK", "KK", "pipi", "Kpipipi", "piKpipi", "pipipipi"};
    } else {
        raw_modes = limited_modes_to_use;
    }
    for (auto run : runs) {
        for (auto mode : raw_modes) {
            if (mode == "pipipipi" && run == "_run1") continue;
            if (split) {
                modes_to_plot.push_back(mode + run + "_plus");
                modes_to_plot.push_back(mode + run + "_minus");
            } else {
                modes_to_plot.push_back(mode + run);
            }
        }
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
            plotter->AddComponent(mode, type + "signal", DrawStyle::Line, kRed + 2);
        }
        plotter->AddComponent(mode, type + "DKpipi", DrawStyle::Filled, kCyan + 2);

        // Add Bs components
        if (!is_favoured) {
            plotter->AddComponent(mode, type + "Bs", DrawStyle::Line, ANAGreen);
            plotter->AddComponent(mode, type + "Bs_low" + run, DrawStyle::Filled, kOrange + 7);
        }

        // Add other backgrounds
        plotter->AddComponent(mode, type + "rho", DrawStyle::Filled, ANAPurple);
    }

    // Add low mass background
    plotter->AddComponent("low", DrawStyle::Filled, kOrange);

    // Draw the plots
    plotter->Draw();
    // delete plotter;

    return 0;
}
