#ifndef PARAMETER_MANAGER_HPP
#define PARAMETER_MANAGER_HPP

#include "RooArgList.h"

#include "ManagerBase.hpp"
#include "ParameterList.hpp"

// =======================================================
// Class for storing RooRealVars and other such parameters
// =======================================================
class ParameterManager : public ManagerBase {

public:
    ParameterManager();
    ParameterManager(std::string name);
    ~ParameterManager();

public:
    void AddRealVar(std::string name, double value);
    void AddRealVar(std::string name, double min, double max);
    void AddRealVar(std::string name, double value, double min, double max);
    void AddRandomisedVar(std::string name, double mean, double width);
    void AddFormulaVar(std::string name, std::string formula, ParameterList params);
    void AddSummedVar(std::string name, std::string var1, std::string var2);
    void AddProductVar(std::string name, std::string var1, std::string var2);
    void AddUnblindVar(std::string name, std::string blind_var, 
            std::string blind_string, double amount);
    void AddResultsFromFile(std::string filename);

};

#endif // PARAMETER_MANAGER_HPP
