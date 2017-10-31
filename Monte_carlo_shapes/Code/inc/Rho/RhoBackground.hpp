#ifndef BACKGROUND_DATA_H
#define BACKGROUND_DATA_H

#include <map>
#include <memory>
#include <string>

#include "TCut.h"

class TLorentzVector;
class TH1F;
class TTree;

class Fitter;

// ==========================================================================
// Class for making histogram of data passing selection and sending to fitter
// ==========================================================================

class RhoBackground {
public:
    RhoBackground(std::string name, bool usePidCuts);
    ~RhoBackground();

    void addData(std::string filename, std::string treename);
    void fitHistogram(std::string paramInput);

    std::string getName();
    int getEvents();
    std::map<std::string, double> getParameters();

private:
    bool m_fitDone;
    TCut m_cut;
    std::string m_name;
    int m_events;
    TH1F* m_histo; 
    std::unique_ptr<Fitter> m_fitter; 
    std::map<std::string, double> m_parameters;

private: 
    TCut readCut(bool usePidCuts);
    void processTree(TTree* tree);
    void LorentzVectorBranch(TTree* tree, TLorentzVector& vec,
            const std::string& name);
};

#endif // BACKGROUND_DATA_H
