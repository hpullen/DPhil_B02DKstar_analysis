// =========================
// Test script for CutReader
// =========================
#include <iostream>
#include "CutReader.hpp"

int main(int argc, char * argv[]) {

    CutReader * cr = new CutReader("Kpi");
    cr->Print();
    TCut cut = cr->GetCut();
    std::cout << "Full cut: " << cut << std::endl;
    cut = cr->GetCut("D0_M ");
    std::cout << "D0 cut: " << cut << std::endl;
    cut = cr->GetCutExcept("PID");
    std::cout << "Without PID cuts: " << cut << std::endl;
    cr->SetMode();
    cr->Print();
    delete cr;

}

