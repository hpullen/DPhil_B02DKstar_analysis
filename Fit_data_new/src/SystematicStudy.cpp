#include "SystematicFitter.hpp"

int main(int argc, char * argv[]) {

    SystematicFitter * fitter = new SystematicFitter();
    fitter->PerformFits("test.root", 1);
    delete fitter;

}
