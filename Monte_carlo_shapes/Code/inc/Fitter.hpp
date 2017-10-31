#ifndef BACKGROUND_FITTER_H
#define BACKGROUND_FITTER_H

#include <map>
#include <string>

#include "TH1F.h"

// =======================================
// Abstract base class for fitting classes
// =======================================

class Fitter {
public:
    Fitter(std::string name);
    virtual ~Fitter();

    virtual std::map<std::string, double> fit(TH1F* hist) = 0;
    virtual void readStartingParams(std::string paramFile) = 0;

protected:
    std::string m_name;

protected:
    void plotFit();
    void plotFit(std::string comp1, std::string comp2);
};

#endif // BACKGROUND_FITTER_H
