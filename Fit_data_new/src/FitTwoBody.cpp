#include <vector>

#include "TwoBodyFitter.hpp"
#include "Plotter.hpp"

using namespace TwoBody;

int main(int argc, char * argv[]) {


    // Make the fitter
    TwoBodyFitter * fitter = new TwoBodyFitter();

    // Years and modes
    std::vector<std::string> years = {"2011", "2012", "2015", "2016"};
    std::vector<Mode> modes = {Mode::Kpi, Mode::piK, Mode::KK, 
        Mode::pipi};

    // Add files
    std::string data_path = "/data/lhcb/users/pullen/B02DKstar/data/twoBody/";
    for (auto mode : modes) {
        for (auto year : years) {
            std::string filepath_down = data_path + year + "_down/" + 
               GetModeString(mode) + "_selected.root";
            std::string filepath_up = data_path + year + "_up/" + 
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

    // Fit
    fitter->PerformFit("Results/twoBody_data.root", "Histograms/twoBody_data.root");

    // Plot
    Plotter * plotter = new Plotter("Histograms/twoBody_data.root",
            "Plots/twoBody_data", {"Kpi", "piK", "KK", "pipi"});

    // Add combinatorial and signal components
    plotter->AddComponent("expo", DrawStyle::Filled, ANABlue, "Combinatorial");
    plotter->AddComponent("Kpi", "signal", DrawStyle::Line, kRed + 2,
            "B^{0}_{d}#rightarrowDK^{*0}");
    plotter->AddComponent("Kpi", "DKpipi", DrawStyle::Filled, kMagenta,
            "B^{+}#rightarrowDK^{+}#pi^{-}#pi^{+}");

    // Add Bs components
    std::vector<std::string> modes_with_Bs = {"piK", "KK", "pipi"};
    for (auto mode : modes_with_Bs) {
        plotter->AddComponent(mode, "Bs_low", DrawStyle::Filled, kOrange + 7,
                "#bar{B}^{0}_{s}#rightarrowD^{*}K^{*0}");
        plotter->AddComponent(mode, "Bs", DrawStyle::Line, ANAGreen,
                "#bar{B}^{0}_{s}#rightarrowDK^{*0}");
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
