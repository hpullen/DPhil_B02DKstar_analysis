#ifndef SIGNAL_DATA_H
#define SIGNAL_DATA_H

#include <map>
#include <memory>
#include <string>

#include "TCut.h"

class TLorentzVector;
class TH1F;
class TTree;

class Fitter;

enum BgType { Bd_signal, Bs_signal, Kpipipi_signal, pipipipi_signal};

enum FitShape { DoubleGaussian, DoubleCB };

// ==========================================================================
// Class for making histogram of data passing selection and sending to fitter
// ==========================================================================

class SignalData {
public:
    SignalData(std::string name, BgType bgType, FitShape fitShape);
    ~SignalData();

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
    TCut readCut(BgType bgType);
    void processTree(TTree* tree);
    void LorentzVectorBranch(TTree* tree, TLorentzVector& vec,
            const std::string& name);
};

#endif // SIGNAL_DATA_H
