#include <iostream>
#include "SystematicFitter.hpp"

int main(int argc, char * argv[]) {

    // Check args
    if (argc != 3) {
        std::cout << "Usage: ./Systematics <number> <type>" << std::endl;
        return -1;
    }

    // Get input option
    std::string number = argv[1];
    std::string opt_str = argv[2];
    std::string output = "/data/lhcb/users/pullen/B02DKstar/systematics/";
    output += opt_str + "/sys_" + number + ".root";
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
    } else if (opt_str == "fs_fd") {
        opt = SysOption::fs_fd;
    } else if (opt_str == "four_vs_two") {
        opt = SysOption::four_vs_two;
    } else if (opt_str == "signal_shape_pars") {
        opt = SysOption::signal_shape_pars;
    } else if (opt_str == "gamma_pi_selection") {
        opt = SysOption::gamma_pi_selection;
    } else if (opt_str == "gamma_pi_branching_ratios") {
        opt = SysOption::gamma_pi_branching_ratios;
    } else if (opt_str == "DKpipi_inputs") {
        opt = SysOption::DKpipi_inputs;
    } else {
        std::cout << "Unrecognised option: " << opt_str << std::endl;
        return -1;
    }

    // Run
    SystematicFitter * fitter = new SystematicFitter(opt);
    fitter->PerformFits(output, 10);

}
