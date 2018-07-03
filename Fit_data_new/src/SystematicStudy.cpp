#include <iostream>
#include "SystematicFitter.hpp"

int main(int argc, char * argv[]) {

    if (argc != 2) {
        std::cout << "Usage: ./Systematics <number>" << std::endl;
        return -1;
    }
    std::string number = argv[1];
    SystematicFitter * fitter = new SystematicFitter();
    fitter->PerformFits("/data/lhcb/users/pullen/B02DKstar/systematics/"
            "low_frac_pi_010/low_frac_010_piK_" + number + ".root", 1);

}
