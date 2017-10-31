#ifndef LOW_MASS_BACKGROUND
#define LOW_MASS_BACKGROUND

#include <map>
#include <string>

#include "TCut.h"
#include "LowMassComponents.hpp"
#include "AllParameters.hpp"

class TLorentzVector;
class TH1F;
class TTree;
class LowMassFitter;
class RangeReader;

class LowMassBackground {

public:
    LowMassBackground(int run_number);
    ~LowMassBackground();

    void addData(std::string filename, std::string treename, Component component);
    void fitHistograms();

    std::string getName();
    int getEvents(Component comp);

private:
    bool m_fitDone;
    TCut m_cut;
    std::string m_name;
    int m_run;
    std::map<Component, BackgroundComponent> m_components;

private:
    TCut readCut();
    std::string getFullName(int run_number);
    std::string compString(Component comp, int run_number);
    void processTree(TTree * tree, Component comp);
    void makeParamsFiles();
    void makeOverallParamsFile();
    void LorentzVectorBranch(TTree * tree, TLorentzVector& vec, const std::string& name);

};

#endif // LOW_MASS_BACKGROUND
