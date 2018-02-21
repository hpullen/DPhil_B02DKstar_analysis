#include "RooRealVar.h"
#include "RooFormulaVar.h"
#include "RooUnblindUniform.h"

#include "ParameterManager.hpp"

// ===================
// Default constructor
// ===================
ParameterManager::ParameterManager() : ManagerBase("parameter_manager") {
}


// ===========
// Constructor
// ===========
ParameterManager::ParameterManager(std::string name) : ManagerBase(name) {
}


// ==========
// Destructor
// ==========
ParameterManager::~ParameterManager() {
}


// ===============================
// Add RooRealVar with fixed value
// ===============================
void ParameterManager::AddRealVar(std::string name, double value) {
    RooRealVar * var = new RooRealVar((m_name + "_" + name).c_str(), "", value);
    AddItem(name, var);
}


// =========================
// Add RooRealVar with range
// =========================
void ParameterManager::AddRealVar(std::string name, double min, double max) {
    RooRealVar * var = new RooRealVar((m_name + "_" + name).c_str(), "", min, max);
    AddItem(name, var);
}


// ============================================
// Add RooRealVar with starting point and range
// ============================================
void ParameterManager::AddRealVar(std::string name, double value, double min, 
        double max) {
    RooRealVar * var = new RooRealVar((m_name + "_" + name).c_str(), "", value, 
            min, max);
    AddItem(name, var);
}


// =================
// Add RooFormulaVar
// =================
void ParameterManager::AddFormulaVar(std::string name, std::string formula,
        ParameterList pars) {

    // Check that all of the parameters in the formula exist
    for (auto par : pars.GetVector()) {
        if (!CheckForExistence(par)) {
            std::cout << "Parameter " << par << " in formula for " << name << 
                " not found in " << m_name << std::endl;
        }
    }

    // Get RooArgList
    RooArgList args = pars.GetArgList(m_map);

    // Make formulavar
    RooFormulaVar * var = new RooFormulaVar((m_name + "_" + name).c_str(), "", 
            formula.c_str(), args);
    AddItem(name, var);

}


// ========================================================
// Add a variable that is the sum of two existing variables
// ========================================================
void ParameterManager::AddSummedVar(std::string name, std::string var1,
        std::string var2) {
    AddFormulaVar(name, "@0 + @1", ParameterList(var1, var2));
}


// ============================================================
// Add a variable that is the product of two existing variables
// ============================================================
void ParameterManager::AddProductVar(std::string name, std::string var1,
        std::string var2) {
    AddFormulaVar(name, "@0 * @1", ParameterList(var1, var2));
}


// ======================================
// Add an unblinded version of a variable
// ======================================
void ParameterManager::AddUnblindVar(std::string name, std::string blind_var,
        std::string blind_string, double amount) {
    RooUnblindUniform * var = new RooUnblindUniform((m_name + "_" + name).c_str(), 
            "", blind_string.c_str(), amount, *Get(blind_var));
    AddItem(name, var);
}
