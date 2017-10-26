#ifndef MVA_APPLICATOR_H
#define MVA_APPLICATOR_H

#include <string>
#include <map>

#include "TMVA/Reader.h"

class TTree;

// =====================================================
// Class for applying an MVA classification to a dataset
// =====================================================
class MVAApplicator {

public:
    MVAApplicator();
    ~MVAApplicator();

    void apply(std::string inputPath, std::string outputPath, std::string mvaName, 
            std::string varFile);

private:
    void setupMVAVariables(TMVA::Reader * reader, std::string varfile, 
            std::map<std::string, Float_t> * vars_map);
    void setupDataVariables(TTree * TTree, std::string varfile,
            std::map<std::string, double> * vars_map, 
            std::map<std::string, float> * floats_map);
    void bookMVA(TMVA::Reader * reader, std::string mvaName);
    void evaluateMVA(TMVA::Reader * reader, TTree * inputTree, TTree * outputTree,
            std::string name, Float_t * response, 
            std::map<std::string, Float_t> * mva_vars, 
            std::map<std::string, double> * data_vars,
            std::map<std::string, float> * float_vars);

};

#endif // MVA_APPLICATOR_H
