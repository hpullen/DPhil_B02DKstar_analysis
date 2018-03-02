#include <iostream>

#include "ParameterReader.hpp"

int main(int argc, char * argv[]) {

    ParameterReader * pr = new ParameterReader("/home/pullen/analysis/B02DKstar/Fit_monte_carlo/Results/");
    pr->ReadParameters("signal", "signal_Kpi.param");
    pr->ReadParameters("test", "before_ratio/test.param");
    std::cout << "frac error: " << pr->GetError("test", "frac") << std::endl;
    std::cout << "frac value: " << pr->GetValue("test", "frac") << std::endl;
    delete pr;

}
