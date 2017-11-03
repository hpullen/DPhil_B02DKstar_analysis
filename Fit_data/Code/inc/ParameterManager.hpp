#ifndef PARAMETER_MANAGER_H
#define PARAMETER_MANAGER_H

#include <map>
#include <string>

#include "RooAbsReal.h"
#include "RooRealVar.h"

enum ParamType { real, other };

// ===============================================
// Class for storing and managing RooFit variables
// ===============================================
class ParameterManager {

public:
    ParameterManager(std::string name);
    ~ParameterManager();

    RooAbsReal * getParam(std::string name);
    RooAbsReal * operator[](std::string name);

    void addParam(std::string name, double value);
    void addParam(std::string name, double low, double high);
    void addParam(std::string name, double start, double low, double high);
    void addParam(std::string name, ParameterManager * params);
    void addParam(std::string name, RooAbsReal * param, 
            ParamType type = ParamType::other);

    void addBlindParam(std::string name, ParameterManager * params, 
            std::string paramToBlind, std::string blindString, double amount = 50);
    void addBlindParam(std::string name, double start, double low, double high,
            std::string blindString, double amount = 50);
    void addFormulaVar(std::string name, std::string formula, 
            std::string var, ParameterManager * params);
    void addFormulaVar(std::string name, std::string formula, 
            std::map<int, std::string> vars, ParameterManager * params);
    void addFormulaVar(std::string name, std::string formula, 
            ParameterManager * params_0, std::string name_0, 
            ParameterManager * params_1, std::string name_1);
    void addFormulaVar(std::string name, std::string formula, 
            std::map<int, std::pair<ParameterManager *, std::string>> vars);
    void addBlindFormula(std::string name, std::string formula, 
            std::map<int, std::pair<ParameterManager *, std::string>> vars,
            std::string blindString, double amount = 50);

    void fixParam(std::string name);
    void fixParam(std::string shapeName, std::string paramName);
    void freeParam(std::string name);
    void freeParam(std::string shapeName, std::string paramName);
    void freeParam(std::string name, double low, double high);
    void freeParam(std::string shapeName, std::string paramName, double low, 
            double high);

    bool isRealVar(std::string name);
    std::string getName();
    void reset();

private:
    std::string m_name;
    std::map<std::string, ParamType> m_params;
    std::map<std::string, RooRealVar*> m_reals;
    std::map<std::string, RooAbsReal*> m_others;

    void checkForDuplicates(std::string name);
    bool checkForExistence(std::string name);
    
    bool changeConst(std::string name, bool isConst);

};


#endif // PARAMETER_MANAGER_H
