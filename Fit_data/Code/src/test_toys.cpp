#include "RooRealVar.h"
#include "ShapeMaker.hpp"

int main(int argc, char * argv[]) {

    RooRealVar * Bd_M = new RooRealVar("Bd_M", "", 5000, 5800);
    ShapeMaker * sm = new ShapeMaker("N", Bd_M);
    sm->makeGenerationPdf("../Results/twoBody_2011:2012:2015:2016_split_binned.root");
    return 0;

}

