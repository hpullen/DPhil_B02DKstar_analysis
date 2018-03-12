#include "CutFitter.hpp"

int main(int argc, char * argv[]) {

    // Make fitter
    CutFitter * fitter = new CutFitter();

    // Years and modes
    // std::vector<std::string> years = {"2011", "2012", "2015", "2016"};
    std::vector<std::string> years = {"2011"};
    std::vector<Data::Mode> modes_twoBody = {Data::Mode::Kpi};
    // std::vector<Data::Mode> modes_fourBody = {Data::Mode::Kpipipi, Data::Mode::piKpipi, Data::Mode::pipipipi};

    // Add two body files
    std::string data_path = "/data/lhcb/users/pullen/B02DKstar/data/twoBody/";
    for (auto mode : modes_twoBody) {
        for (auto year : years) {
            std::string filepath_down = data_path + year + "_down/" +
                GetModeString(mode) + "_selected.root";
            // std::string filepath_up = data_path + year + "_up/" +
                // GetModeString(mode) + "_selected.root";
            fitter->AddFile(mode, filepath_down);
            // fitter->AddFile(mode, filepath_up);
        }
    }

    // // Add four body files
    // std::string data_path_fourBody = "/data/lhcb/users/pullen/B02DKstar/data/fourBody/";
    // for (auto mode : modes_fourBody) {
        // for (auto year : years) {
            // if (mode == Data::Mode::pipipipi && (year == "2011" || year == "2012")) {
                // continue;
            // }
            // std::string filepath_down = data_path_fourBody + year + "_down/" +
                // GetModeString(mode) + "_selected.root";
            // std::string filepath_up = data_path_fourBody + year + "_up/" +
                // GetModeString(mode) + "_selected.root";
            // fitter->AddFile(mode, filepath_down);
            // fitter->AddFile(mode, filepath_up);
        // }
    // }


    // Set cuts
    fitter->SetCut(Data::Mode::Kpi, 0.5, 0.9, 0.1);

    // Perform fit
    fitter->PerformStudy("Results/BDT/test.root");

}
