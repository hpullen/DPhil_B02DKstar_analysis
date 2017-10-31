#include <iostream>

#include "LowMass/ProcessLowMassBackground.hpp"

int main(int argc, char * argv[]) {

    ProcessLowMassBackground * processer = new ProcessLowMassBackground();
    processer->process();
    delete processer;

    return 0;

}
