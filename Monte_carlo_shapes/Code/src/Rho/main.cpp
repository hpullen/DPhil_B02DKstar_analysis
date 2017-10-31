#include <iostream>

#include "Rho/ProcessRho.hpp"

int main (int argc, char* argv[]) {

    ProcessRho* processer = new ProcessRho;
    processer->processAll();
    delete processer;

    return 0;
}
