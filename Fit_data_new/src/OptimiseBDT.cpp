#include "CutFitter.hpp"
#include "DataPdfMaker.hpp"

struct CutOption {
    bool single;
    double value;
    double min;
    double max;
    double increment;
};

int main(int argc, char * argv[]) {

    // Years and modes
    std::vector<std::string> years = {"2011", "2012", "2015", "2016"};
    std::vector<std::string> modes_twoBody = {"Kpi", "piK", "KK", "pipi"};
    std::vector<std::string> modes_fourBody = {"Kpipipi", "piKpipi", "pipipipi"};

    // Check usage
    std::map<std::string, CutOption> cut_opts;
    if (argc < 3) {
        std::cout << "Usage: ./StudyBDT -mode cut -mode2 cut_min cut_max increment"
            << std::endl;
        return -1;
    }

    // Parse options
    int i = 1;
    std::string current_mode;
    while (i < argc) {

        // Get mode and subsequent args
        if (std::string(argv[i]).find("-") != 0) {
            std::cout << "Parse error on arg " << i << "!" << std::endl;
            return -1;
        }
        current_mode = std::string(argv[i]).substr(1, std::string::npos);
        std::string first_arg = argv[i + 1];
        std::string second_arg = argv[i + 2];

        // Check if a cut range is given
        if (second_arg.find("-") == 0) {
            cut_opts[current_mode].single = true;
            cut_opts[current_mode].value = atof(argv[i + 1]);
            i += 2;
        } else {
            cut_opts[current_mode].single = false;
            cut_opts[current_mode].min = atof(argv[i + 1]);
            cut_opts[current_mode].max = atof(argv[i + 2]);
            cut_opts[current_mode].increment = atof(argv[i + 3]);
            i += 4;
        }
    }

    // Check options are ok
    for (auto opt : cut_opts) {

        // Check mode is valid
        bool valid = false;
        for (auto mode : modes_twoBody) {
            if (opt.first == mode) valid = true;
        }
        for (auto mode : modes_fourBody) {
            if (opt.first == mode) valid = true;
        }
        if (!valid) {
            std::cout << "Mode " << opt.first << " is not a valid mode!" << 
                std::endl;
            return -1;
        }
    }

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
    for (auto opt : cut_opts) {
        if (opt.second.single) {
            fitter->SetCut(opt.first, opt.second.value);
        } else {
            fitter->SetCut(opt.first, opt.second.min, opt.second.max, 
                    opt.second.increment);
        }
    }


    // Perform fit
    fitter->PerformStudy("Results/BDT/test.root");

    // Clean up
    delete fitter;
    delete pdf;
}
