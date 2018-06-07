#include <iostream>
#include <vector>

#include "CrossCheckPdfMaker.hpp"
#include "DataFitter.hpp"
#include "Plotter.hpp"

int main(int argc, char * argv[]) {

    // Make PDF maker
    RooCategory * cat = new RooCategory("category", "");
    for (TString mode : {"Kpi", "piK", "KK", "pipi", "Kpipipi", "piKpipi", 
            "pipipipi"}) {
        for (TString run : {"_run1", "_run2"}) {
            if (mode == "pipipipi" && run == "_run1") continue;
            cat->defineType(mode + run);
        }
    }
    RooRealVar * Bd_M = new RooRealVar("Bd_ConsD_MD", "", 5000, 5800);
    int binWidth = 8;
    double nBins = (Bd_M->getMax() - Bd_M->getMin()) / binWidth;
    Bd_M->setBins(nBins);
    CrossCheckPdfMaker * pdf = new CrossCheckPdfMaker(Bd_M, cat);

    // Make fitter
    DataFitter * fitter = new DataFitter(pdf, false);

    // Years and modes
    std::vector<std::string> years = {"2011", "2012", "2015", "2016"};
    std::vector<std::string> modes_twoBody = {"Kpi", "piK", "KK", "pipi"};
    std::vector<std::string> modes_fourBody = {"Kpipipi", "piKpipi", "pipipipi"};

    // Add two body files
    std::string data_path = "/data/lhcb/users/pullen/B02DKstar/data/twoBody/";
    for (auto mode : modes_twoBody) {
        for (std::string mag : {"up", "down"}) {
            for (std::string year : {"2011", "2012"}) {
                fitter->AddFile(mode + "_run1", data_path + year + "_" + mag + "/"
                        + mode + "_selected.root");
            }
            for (std::string year : {"2015", "2016"}) {
                fitter->AddFile(mode + "_run2", data_path + year + "_" + mag + "/"
                        + mode + "_selected.root");
            }
        }
    }

    // Add four body files
    std::string data_path_fourBody = "/data/lhcb/users/pullen/B02DKstar/data/fourBody/";
    for (auto mode : modes_fourBody) {
        for (std::string mag : {"up", "down"}) {
            for (std::string year : {"2011", "2012"}) {
                if (mode != "pipipipi") {
                    fitter->AddFile(mode + "_run1", data_path_fourBody + year + "_" + mag + "/"
                            + mode + "_selected.root");
                }
            }
            for (std::string year : {"2015", "2016"}) {
                fitter->AddFile(mode + "_run2", data_path_fourBody + year + "_" + mag + "/"
                        + mode + "_selected.root");
            }
        }
    }

    // Filenames
    std::string results_file = "Results/crossCheck.root";
    std::string hist_file = "Histograms/crossCheck.root";
    std::string plot_file = "Plots/crossCheck";

    // Fit
    fitter->PerformFit(results_file);
    pdf->SaveHistograms(hist_file, fitter->GetData());

    // Plot
    std::vector<std::string> modes_to_plot;
    for (std::string run : {"_run1", "_run2"}) {
        for (std::string mode : {"Kpi", "piK", "KK", "pipi", "Kpipipi", "piKpipi",
                "pipipipi"}) {
            if (!(mode == "pipipipi" && run == "_run1")) {
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
            plotter->AddComponent(mode, type + "signal", DrawStyle::Line, kRed + 2,
                    "B^{0}_{d}#rightarrowDK^{*0}");
        }
        plotter->AddComponent(mode, type + "DKpipi", DrawStyle::Filled,
                kCyan + 2, "B^{+}#rightarrowDK^{+}#pi^{-}#pi^{+}");

        // Add Bs components
        if (!is_favoured) {
            plotter->AddComponent(mode, type + "Bs", DrawStyle::Line, 
                    ANAGreen, "#bar{B}^{0}_{s}#rightarrowDK^{*0}");
            plotter->AddComponent(mode, type + "Bs_low" + run, DrawStyle::Filled, 
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
