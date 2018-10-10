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
    if (argc < 2) {
        std::cout << "Usage: ./StudyBDT <tree-file> -mode1 <cut> -mode2 <cut_min> "
            << "<cut_max> <increment>" << std::endl;
        return -1;
    }

    // Parse options
    std::string tree_file = argv[1];
    int i = 2;
    std::string current_mode;
    while (i < argc) {

        // Get mode and subsequent args
        if (std::string(argv[i]).find("-K") != 0 && std::string(argv[i]).find("-pi") != 0) {
            std::cout << "Parse error on arg " << i << "!" << std::endl;
            return -1;
        }
        current_mode = std::string(argv[i]).substr(1, std::string::npos);
        std::string first_arg = argv[i + 1];
        bool single_cut = false;
        if (i + 2 >= argc) {
            single_cut = true;
        } else {
            std::string second_arg = argv[i + 2];
            single_cut = second_arg.find("-K") == 0 || second_arg.find("-pi") == 0;
        }

        // Check if a cut range is given
        if (single_cut) {
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
    std::vector<std::string> runs = {"_run1", "_run2"}; 
    for (auto mode : modes_twoBody) {
        for (auto run : runs) {
            cat->defineType((mode + run + "_plus").c_str());
            cat->defineType((mode + run + "_minus").c_str());
        }
    }
    for (auto mode : modes_fourBody) {
        for (auto run : runs) {
            if (mode == "pipipipi" && run == "_run1") continue;
            cat->defineType((mode + run + "_plus").c_str());
            cat->defineType((mode + run + "_minus").c_str());
        }
    }

    // Make fit variable
    RooRealVar * Bd_M = new RooRealVar("Bd_ConsD_MD", "", 5000, 5800);

    // Make shape and cut fitter
    DataPdfMaker * pdf = new DataPdfMaker("data_BDT", Bd_M, cat, true);
    CutFitter * fitter = new CutFitter(pdf);

    // Add two body files
    std::string data_path = "/data/lhcb/users/pullen/B02DKstar/data/twoBody/";
    std::string data_path_fourBody = "/data/lhcb/users/pullen/B02DKstar/data/fourBody/";
    for (auto year : years) {
        std::string run = (year == "2011" || year == "2012") ? "_run1" : "_run2";
        for (auto mode : modes_twoBody) {
            for (std::string pol : {"up", "down"}) {
                std::string filepath = data_path + year + "_" + pol + "/" + mode
                    + "_selected_no_BDT_cut.root";
                fitter->AddFile(mode + run, filepath);
            }
        }
        for (auto mode : modes_fourBody) {
            if (mode == "pipipipi" && run == "_run1") {
                continue;
            }
            for (std::string pol : {"up", "down"}) {
                std::string filepath = data_path_fourBody + year + "_" + pol + "/"
                    + mode + "_selected_no_BDT_cut.root";
                fitter->AddFile(mode + run, filepath);
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
    fitter->PerformStudy(tree_file);

    // Clean up
    delete fitter;
    delete pdf;
}
