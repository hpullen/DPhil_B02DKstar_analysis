#include "ToySignificanceFitter.hpp"

// ==============================================
// Script for estimating significance of data fit
// ==============================================
int main(int argc, char * argv[]) {

    // Get a number to ID the file
    if (argc != 3 && argc != 4) {
        std::cout << "Usage: ./Significance <outfile> <mode> (<BDT cut>)" << std::endl;
        return -1;
    }
    // std::string number = std::string(argv[1]);
    std::string outfile = argv[1];
    std::string mode = argv[2];
    bool BDT_cut = false;
    double cut_val = 0.5;
    if (argc == 4) {
        BDT_cut = true;
        cut_val = atof(argv[3]);
    }

    // Make toy significance fitter
    ToySignificanceFitter * tf;
    if (BDT_cut) {
        tf = new ToySignificanceFitter(mode, cut_val);
    } else {
        tf = new ToySignificanceFitter(mode);
    }
    tf->PerformFits(outfile, 5);

    return 0;
}
