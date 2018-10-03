#include "ToySignificanceFitter.hpp"

// ==============================================
// Script for estimating significance of data fit
// ==============================================
int main(int argc, char * argv[]) {

    // Get a number to ID the file
    if (argc != 1 && argc != 2) {
        std::cout << "Usage: ./Significance (<mode=piK>)" << std::endl;
        return -1;
    }
    // std::string number = std::string(argv[1]);
    std::string mode = (argc == 2) ? argv[1] : "piK";

    // Make toy significance fitter
    ToySignificanceFitter * tf = new ToySignificanceFitter(mode);
    tf->PerformFits("Results/Significance/significance_" + mode + ".root", 1);

    return 0;
}
