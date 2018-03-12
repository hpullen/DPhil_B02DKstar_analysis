#include "ToySignificanceFitter.hpp"

// ==============================================
// Script for estimating significance of data fit
// ==============================================
int main(int argc, char * argv[]) {

    // Get a number to ID the file
    if (argc != 2 && argc != 3) {
        std::cout << "Usage: ./Significance <run-number> (<mode=piK>)" << std::endl;
        return -1;
    }
    std::string number = std::string(argv[1]);
    std::string mode = (argc == 3) ? argv[2] : "piK";

    // Make toy significance fitter
    ToySignificanceFitter * tf = new ToySignificanceFitter(mode);
    tf->PerformFits("Results/Significance/significance_" + mode + "_" + number + 
            ".root", 20);

    return 0;
}
