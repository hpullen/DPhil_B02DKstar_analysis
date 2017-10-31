#ifndef LOW_MASS_FITTER_H
#define LOW_MASS_FITTER_H

#include <map>
#include <string>

#include "LowMassComponents.hpp"
#include "LowMassParameters.hpp"
#include "AllParameters.hpp"

// Forward declarations
class TH1F;
class RooRealVar;
class RooDataHist;
class RooHist;
class RooAbsPdf;

// Class for fitting low mass background
class LowMassFitter {

public:
    LowMassFitter(std::string name, std::map<Component, BackgroundComponent> 
            components, int run_number);
    ~LowMassFitter();

    std::map<Component, AllParameters::LowMassOutput> getParameters();

private:
    int m_run;
    std::string m_name;
    std::map<Component, BackgroundComponent> m_components;

    // Containers for input/output parameters
    // LowMassParameters::InputParameters m_inputParams;
    AllParameters::LowMassShapes m_inputParams;
    std::map<Component, AllParameters::LowMassOutput> m_outputParams;

    // Functions
    void processFit();
    void readStartingParams();
    void performFit();
    RooHist * getPulls(RooRealVar * x, RooDataHist * hData, RooAbsPdf * hFit);
    void plotFits();
    void plotSingleFit(Component comp);

};

#endif // LOW_MASS_FITTER_H
