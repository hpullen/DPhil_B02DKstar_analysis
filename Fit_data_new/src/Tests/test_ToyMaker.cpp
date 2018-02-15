#include "RooRealVar.h"
#include "RooSimultaneous.h"

// ========================================
// Script for testing ParameterManager code
// ========================================
#include <iostream>

#include "SimpleToyMaker.hpp"

int main(int argc, char * argv[]) {

    // ParameterManager * pm = new ParameterManager("test");
    // pm->AddRealVar("a", 3);
    // pm->AddRealVar("b", 6);
    // pm->AddFormulaVar("c", "@1 / @0", ParameterList("a", "b"));
    // std::cout << "Value of c: " << pm->Get("c")->getVal() << std::endl;
    // delete pm;

    // Make mass variable
    RooRealVar * Bd_M = new RooRealVar("Bd_M", "", 5000, 5800);

    // Make category
    RooCategory * cat = new RooCategory("modes", "");
    cat->defineType("Kpi");
    cat->defineType("piK");

    // Make toy maker
    SimpleToyMaker * tm = new SimpleToyMaker("test", Bd_M, cat);
    RooSimultaneous * simPdf = tm->Shape();
    tm->SaveHistograms("test.root");

    delete tm;

}
