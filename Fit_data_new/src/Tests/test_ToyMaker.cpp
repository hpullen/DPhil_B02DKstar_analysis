#include "RooRealVar.h"

// ========================================
// Script for testing ParameterManager code
// ========================================
#include <iostream>

#include "ParameterManager.hpp"


int main(int argc, char * argv[]) {

    ParameterManager * pm = new ParameterManager("test");
    pm->AddRealVar("a", 3);
    pm->AddRealVar("b", 6);
    pm->AddFormulaVar("c", "@1 / @0", ParameterList("a", "b"));
    std::cout << "Value of c: " << pm->Get("c")->getVal() << std::endl;
    delete pm;


}
