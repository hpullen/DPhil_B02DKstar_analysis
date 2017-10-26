#include <string>
#include <iostream>

#include "TFile.h"
#include "TH1D.h"

#include "CompareRoc.hpp"

int main(int argc, char * argv[]) {

    // Check for even number of inputs
    if (argc < 4) {
        std::cout << "Usage: ./CompareRoc <plotname> <mva1> <mva2> ..." 
            << std::endl; 
        return -1;
    }

    // Make a comparison class
    CompareRoc * cr = new CompareRoc();

    // Loop through args and add to comparison
    for (int i = 2; i < argc; i ++) {
        std::string mva = argv[i];
        cr->addCurve(mva, "BDTG");
    }

    // Plot the curves
    std::string plotname= argv[1];
    cr->plotCurves(plotname);
    delete cr;
}

