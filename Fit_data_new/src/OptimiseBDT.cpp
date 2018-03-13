#include "CutFitter.hpp"
#include "DataPdfMaker.hpp"

int main(int argc, char * argv[]) {

    // Years and modes
    std::vector<std::string> years = {"2011", "2012", "2015", "2016"};
    std::vector<std::string> modes_twoBody = {"Kpi", "piK", "KK", "pipi"};
    std::vector<std::string> modes_fourBody = {"Kpipipi", "piKpipi", "pipipipi"};

    // Make category
    RooCategory * cat = new RooCategory("cat", "");
    for (auto mode : modes_twoBody) {
        cat->defineType((mode + "_plus").c_str());
        cat->defineType((mode + "_minus").c_str());
    }
    for (auto mode : modes_fourBody) {
        cat->defineType((mode + "_plus").c_str());
        cat->defineType((mode + "_minus").c_str());
    }

    // Make fit variable
    RooRealVar * Bd_M = new RooRealVar("Bd_ConsD_MD", "", 5000, 5800);

    // Make shape and cut fitter
    DataPdfMaker * pdf = new DataPdfMaker("data_BDT", Bd_M, cat, true);
    CutFitter * fitter = new CutFitter(pdf); 

    // Add two body files
    std::string data_path = "/data/lhcb/users/pullen/B02DKstar/data/twoBody/";
    for (auto mode : modes_twoBody) {
        for (auto year : years) {
            for (std::string pol : {"up", "down"}) {
                std::string filepath = data_path + year + "_" + pol + "/" + mode
                    + "_selected.root";
                fitter->AddFile(mode, filepath);
            }
        }
    }

    // Add four body files
    std::string data_path_fourBody = "/data/lhcb/users/pullen/B02DKstar/data/fourBody/";
    for (auto mode : modes_fourBody) {
        for (auto year : years) {
            if (mode == "pipipipi" && (year == "2011" || year == "2012")) {
                continue;
            }
            for (std::string pol : {"up", "down"}) {
                std::string filepath = data_path_fourBody + year + "_" + pol + "/" 
                    + mode + "_selected.root";
                fitter->AddFile(mode, filepath);
            }
        }
    }

    // Set cuts
    // fitter->SetCut("Kpi", 0.5, 0.9, 0.1);
    fitter->SetCut("Kpi", 0.7);

    // Perform fit
    fitter->PerformStudy("Results/BDT/test.root");

    // Clean up
    delete fitter;
    delete pdf;
}
