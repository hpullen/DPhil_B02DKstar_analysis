#include "ToySignificanceFitter.hpp"

// ==============================================
// Script for estimating significance of data fit
// ==============================================
int main(int argc, char * argv[]) {

    // Make toy significance fitter
    ToySignificanceFitter * tf = new ToySignificanceFitter(Fitter::ToyType::TwoAndFourBody);
    tf->PerformFits("Results/Significance/sig.root", 1);

    return 0;
}
