#include <vector>

#include "TwoBodyFitter.hpp"

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
    fitter->PerformFit("test_result.root", "Histograms/test.root");

    return 0;
}
