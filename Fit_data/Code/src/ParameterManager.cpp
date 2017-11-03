#include <iostream> 

#include "RooRealVar.h"
#include "RooFormulaVar.h"
#include "RooUnblindUniform.h"

#include "ParameterManager.hpp"


// ===========
// Constructor
// ===========
ParameterManager::ParameterManager(std::string name) : m_name(name) {}


// ==========
// Destructor
// ==========
ParameterManager::~ParameterManager() {}


// ==================
// Return a parameter
// ==================
RooAbsReal * ParameterManager::getParam(std::string name) {
    checkForExistence(name);
    return (m_params[name] == ParamType::real) ? 
        (RooAbsReal*)m_reals[name] : m_others[name];
}


// =========================
// Return parameter using []
// =========================
RooAbsReal * ParameterManager::operator[](std::string name) {
    return getParam(name);
}


// ========================
// Add a constant parameter
// ========================
void ParameterManager::addParam(std::string name, double value) {
    checkForDuplicates(name);
    RooRealVar * var = new RooRealVar((m_name + "_" + name).c_str(), "", value);    
    m_params.emplace(name, ParamType::real);
    m_reals.emplace(name, var);
}


// ==========================
// Add a parameter with range
// ==========================
void ParameterManager::addParam(std::string name, double low, double high) {
    checkForDuplicates(name);
    RooRealVar * var = new RooRealVar((m_name + "_" + name).c_str(), "", low, high);
    m_params.emplace(name, ParamType::real);
    m_reals.emplace(name, var);
}


// ===========================================
// Add parameter with range and starting value
// ===========================================
void ParameterManager::addParam(std::string name, double start, double low, 
        double high) {
    checkForDuplicates(name);
    RooRealVar * var = new RooRealVar((m_name + "_" + name).c_str(), "", start, low, 
            high);
    m_params.emplace(name, ParamType::real);
    m_reals.emplace(name, var);
}


// ============================================
// Add parameter from another parameter manager
// ============================================
void ParameterManager::addParam(std::string name, ParameterManager * params) {
    ParamType type;
    if (params->isRealVar(name)) {
        type = ParamType::real;
        m_reals[name] = (RooRealVar*)params->getParam(name);
    } else {
        type = ParamType::other;
        m_others[name] = params->getParam(name);
    }
    m_params.emplace(name, type);
}


// ===============================
// Add preexisting RooFit variable
// ===============================
void ParameterManager::addParam(std::string name, RooAbsReal * param, 
        ParamType type) {
    checkForDuplicates(name);
    param->SetName((m_name + "_" + name).c_str());
    m_params.emplace(name, type);
    if (type == ParamType::real) {
        m_reals[name] = (RooRealVar*)param;
    } else if (type == ParamType::other) {
        m_others[name] = param;
    }
}


// ==========================================================
// Add formula variable with a single variable in the formula
// ==========================================================
void ParameterManager::addFormulaVar(std::string name, std::string formula,
        std::string var, ParameterManager * params) {
    checkForDuplicates(name);

    // Make variable
    RooFormulaVar * formvar = new RooFormulaVar((m_name + "_" + name).c_str(), "",
            formula.c_str(), RooArgList(*params->getParam(var)));
    m_params.emplace(name, ParamType::other);
    m_others.emplace(name, formvar);
}



// ====================
// Add formula variable
// ====================
void ParameterManager::addFormulaVar(std::string name, std::string formula,
        std::map<int, std::string> vars, ParameterManager * params) {
    checkForDuplicates(name);

    // Fill RooArgList of variables
    int n = vars.size();
    RooArgList var_list;
    for (int i = 0; i < n; i++) {
        var_list.add(*params->getParam(vars[i]));
    }

    // Make variable
    RooFormulaVar * formvar = new RooFormulaVar((m_name + "_" + name).c_str(), "",
            formula.c_str(), var_list);
    m_params.emplace(name, ParamType::other);
    m_others.emplace(name, formvar);
}


// ==========================================================================
// Add formula variable using variables from two different parameter managers
// ==========================================================================
void ParameterManager::addFormulaVar(std::string name, std::string formula,
        ParameterManager * params_0, std::string name_0,
        ParameterManager * params_1, std::string name_1) {
    RooFormulaVar * formvar = new RooFormulaVar((m_name + "_" + name).c_str(), "",
            formula.c_str(), RooArgList(*params_0->getParam(name_0), 
                *params_1->getParam(name_1)));
    m_params.emplace(name, ParamType::other);
    m_others.emplace(name, formvar);
}


// =============================================================================
// Add formula manager with arbitrary number of variables and parameter managers
// =============================================================================
void ParameterManager::addFormulaVar(std::string name, std::string formula,
        std::map<int, std::pair<ParameterManager *, std::string>> vars) {
    // Fill RooArgList with variables
    int n = vars.size();
    RooArgList var_list;
    for (int i = 0; i < n; i++) {
        var_list.add(*vars[i].first->getParam(vars[i].second));
    }

    // Make variable
    RooFormulaVar * formvar = new RooFormulaVar((m_name + "_" + name).c_str(), "",
            formula.c_str(), var_list);
    m_params.emplace(name, ParamType::other);
    m_others.emplace(name, formvar);
}


// =================================
// Add formula variable and blind it
// =================================
void ParameterManager::addBlindFormula(std::string name, std::string formula,
        std::map<int, std::pair<ParameterManager *, std::string>> vars,
        std::string blindString, double amount) {
    this->addFormulaVar(name + "_blind", formula, vars);
    this->addBlindParam(name, this, name + "_blind", blindString, amount);

}


// ====================
// Add blinded variable
// ====================
void ParameterManager::addBlindParam(std::string name, ParameterManager * params,
        std::string paramToBlind, std::string blindString, double amount) {
    RooUnblindUniform * blind = new RooUnblindUniform((m_name + "_" + name).c_str(), 
            "", blindString.c_str(), amount, *params->getParam(paramToBlind));
    m_params.emplace(name, ParamType::other);
    m_others.emplace(name, blind);
}


// ============================
// Create variable and blind it
// ============================
void ParameterManager::addBlindParam(std::string name, double start, double low,
        double high, std::string blindString, double amount) {
    this->addParam(name + "_blind", start, low, high);
    this->addBlindParam(name, this, name + "_blind", blindString, amount);
}


// ===============
// Fix a parameter
// ===============
void ParameterManager::fixParam(std::string name) {
    changeConst(name, true);
}

void ParameterManager::fixParam(std::string shapeName, std::string paramName) {
    std::string fullname = shapeName + "_" + paramName;
    changeConst(fullname, true);
}


// ================
// Free a parameter
// ================
void ParameterManager::freeParam(std::string name) {
    changeConst(name, false);
}

void ParameterManager::freeParam(std::string shapeName, std::string paramName) {
    std::string fullname = shapeName + "_" + paramName;
    changeConst(fullname, false);
}


// ========================================
// Free and parameter and change its limits
// ========================================
void ParameterManager::freeParam(std::string name, double low, double high) {
    bool result = changeConst(name, false);
    if (result) {
        m_reals[name]->setMin(low);
        m_reals[name]->setMax(high);
    }
}

void ParameterManager::freeParam(std::string shapeName, std::string paramName, double low, 
        double high) {
    std::string fullname = shapeName + "_" + paramName;
    bool result = changeConst(fullname, false);
    if (result) {
        m_reals[fullname]->setMin(low);
        m_reals[fullname]->setMax(high);
    }
}




// ======================================
// Check whether parameter already exists
// ======================================
void ParameterManager::checkForDuplicates(std::string name) {
    if (m_params.count(name)) {
        std::cout << "Warning: parameter " << name << " already exists in parameter"
            " manager " << m_name << "!" << std::endl;
    }
}


// =========================
// Check if parameter exists
// =========================
bool ParameterManager::checkForExistence(std::string name) {
    bool exists = true;
    if (!m_params.count(name)) {
        std::cout << "Warning: parameter " << name << " does not exist in " <<
            m_name << std::endl;
        exists = false;
    }
    return exists;
}


// =============================================
// Change whether or not a parameter is constant
// =============================================
bool ParameterManager::changeConst(std::string name, bool isConst) {
    if (!checkForExistence(name)) {
        return false;
    }
    if (!isRealVar(name)) {
        std::cout << name << " is not a RooRealVar: cannot use setConstant!" <<
            std::endl;
        return false;
    }
    m_reals[name]->setConstant(isConst);
    return true;
}


// ===================================
// Check if a variable is a RooRealVar
// ===================================
bool ParameterManager::isRealVar(std::string name) {
    if (!checkForExistence(name)) {
        return false;
    }
    return (m_params[name] == ParamType::real);
}


std::string ParameterManager::getName() {
    return m_name;
}


void ParameterManager::reset() {
    m_params.clear();
}
