#include "ToySignificanceFitter.hpp"

// ==============================================
// Script for estimating significance of data fit
// ==============================================
int main(int argc, char * argv[]) {

    // Get a number to ID the file
    if (argc != 2) {
        std::cout << "Usage: ./FitterBias <run-number>" << std::endl;
        return -1;
    }
    std::string number = std::string(argv[1]);

    // Make toy significance fitter
    ToySignificanceFitter * tf = new ToySignificanceFitter(Fitter::ToyType::TwoAndFourBody);
    tf->PerformFits("Results/Significance/significance_piKpipi_" + number + ".root", 10);

    return 0;
}
