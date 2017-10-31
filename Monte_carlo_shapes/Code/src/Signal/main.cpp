#include <iostream>

#include "Signal/ProcessSignal.hpp"

int main (int argc, char* argv[]) {

    ProcessSignal* processer = new ProcessSignal;
    processer->processAll();
    delete processer;

    return 0;
}
