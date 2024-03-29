#include <iostream>
#include "SystematicFitter.hpp"
#include "ToySystematicFitter.hpp"

int main(int argc, char * argv[]) {

    // Check args
    if (argc != 3 && argc != 4) {
        std::cout << "Usage: ./Systematics <number/TEST> <type> (--combinedRuns)" << std::endl;
        return -1;
    }
    bool combine_runs = false;
    bool split_obs = false;
    bool binned = true;
    if (argc > 3) {
        for (int i = 3; i < argc; i++) {
            std::string arg = argv[i];
            if (arg == "--combineRuns") {
                combine_runs = true;
                std::cout << "Will combine run 1 + run 2" << std::endl;
            } else if (arg == "--splitObs") {
                split_obs = true;
                std::cout << "Will split oberservables by run" << std::endl;
            } else if (arg == "--unbinned") {
                binned = false;
                std::cout << "Will perform unbinned fit" << std::endl;
            } else {
                std::cout << "Unrecognised option " << argc << std::endl;
            }
        }
    }

    // Get input option
    std::string number = argv[1];
    std::string opt_str = argv[2];
    std::string opt_dir = "";
    if (combine_runs) opt_dir +=  "CombinedRuns/";
    if (split_obs) opt_dir += "SplitObs/";
    if (!binned) opt_dir += "Unbinned/";
    opt_dir += opt_str;

    // See if it's a test run
    bool test = (number == "TEST");
    std::string output = "/data/lhcb/users/pullen/B02DKstar/systematics/";
    if (test) {
        output = "test/" + opt_dir + "_test.root";
        std::cout << "Running a test" << std::endl;
    } else {
        output += opt_dir + "/sys_" + number + ".root";
    }

    // Get option
    SysOption opt;
    if (opt_str == "branching_ratios") {
        opt = SysOption::branching_ratios;
    } else if (opt_str == "selection_efficiency") {
        opt = SysOption::selection_efficiency;
    } else if (opt_str == "production_asymmetry") {
        opt = SysOption::production_asymmetry;
    } else if (opt_str == "detection_asymmetry") {
        opt = SysOption::detection_asymmetry;
    } else if (opt_str == "fixed_rho") {
        opt = SysOption::fixed_rho;
    } else if (opt_str == "rho_PID") {
        opt = SysOption::rho_PID;
    } else if (opt_str == "fs_fd") {
        opt = SysOption::fs_fd;
    } else if (opt_str == "four_vs_two") {
        opt = SysOption::four_vs_two;
    } else if (opt_str == "signal_shape_pars") {
        opt = SysOption::signal_shape_pars;
    } else if (opt_str == "Bs_low_shape_pars") {
        opt = SysOption::Bs_low_shape_pars;
    } else if (opt_str == "background_shape_pars") {
        opt = SysOption::background_shape_pars;
    } else if (opt_str == "gamma_pi_inputs") {
        opt = SysOption::gamma_pi_inputs;
    } else if (opt_str == "DKpipi_inputs") {
        opt = SysOption::DKpipi_inputs;
    } else if (opt_str == "PID") {
        opt = SysOption::PID;
    } else if (opt_str == "delta_M") {
        opt = SysOption::delta_M;
    } else if (opt_str == "pipipipi_low") {
        opt = SysOption::pipipipi_low;
    } else if (opt_str == "charmless") {
        opt = SysOption::charmless;
    } else {
        std::cout << "Unrecognised option: " << opt_str << std::endl;
        return -1;
    }

    // Run
    int n_runs = test ? 1 : 10;
    if (opt == SysOption::charmless) {

        // Toy fits with added charmless events
        std::cout << "Combine runs variable: " << combine_runs << std::endl;
        ToySystematicFitter * fitter = new ToySystematicFitter(opt, combine_runs, 
                split_obs);
        fitter->PerformFits(output, n_runs);

    } else {

        // Normal study with many data fits
        SystematicFitter * fitter = new SystematicFitter(opt, combine_runs, split_obs);
        fitter->PerformFits(output, n_runs, binned);
    }
}
