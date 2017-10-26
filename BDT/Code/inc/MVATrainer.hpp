#ifndef NEW_MVA_TRAINER_H
#define NEW_MVA_TRAINER_H

#include <string>
#include <vector>

#if not defined(__CINT__) || defined(__MAKECINT__)
#include "TMVA/Factory.h"
#include "TMVA/Tools.h"
#endif

// =======================
// Class for training MVAs
// =======================
class MVATrainer {

public:
    MVATrainer();
    MVATrainer(int nTrees, int maxDepth);
    ~MVATrainer();

    void train(std::string name, std::vector<std::string> signalFiles,
            std::vector<std::string> backgroundFiles, std::string varFile,
            std::string cutFile);
    
private:
    int m_nTrees;
    int m_maxDepth;

    void addVariables(TMVA::Factory * factory, std::string varFile);
    void addTrees(TMVA::Factory * factory, std::vector<std::string> signalFiles,
            std::vector<std::string> backgroundFiles, std::string cutFile);
    void bookMethods(TMVA::Factory * factory, std::string name);
};

#endif // NEW_MVA_TRAINER_H
