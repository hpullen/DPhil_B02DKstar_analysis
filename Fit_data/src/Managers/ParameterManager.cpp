#include <fstream>
#include <string> 
#include <algorithm>

#include "TFile.h"
#include "TRandom.h"

#include "RooRealVar.h"
#include "RooFormulaVar.h"
#include "RooUnblindUniform.h"
#include "RooFitResult.h"

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


// =====================================================
// Add a variable shifted about its mean with a Gaussian
// =====================================================
void ParameterManager::AddRandomisedVar(std::string name, double mean, double width) {
    double value = -1;
    if (mean > 0) {
        while (value < 0) {
            gRandom->SetSeed(0);
            value = gRandom->Gaus(mean, width);
            std::cout << "Shifted " << name << " from " << mean << " to " <<  value << std::endl;
            std::cout << "(width = " << width << ")" << std::endl;
        }
    } else {
        gRandom->SetSeed(0);
        value = gRandom->Gaus(mean, width);
        std::cout << "Shifted " << name << " from " << mean << " to " <<  value << std::endl;
        std::cout << "(width = " << width << ")" << std::endl;
    }
    AddRealVar(name, value);
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
    m_blindVars.push_back(blind_var);
    m_blindVars.push_back(name);
    AddItem(name, var);
}


// ================================================
// Load in parameters from a RooFitResult in a file
// ================================================
void ParameterManager::AddResultsFromFile(std::string filename, bool warnings) {
    
    // Open file and get RooFitResult
    TFile * file = TFile::Open(filename.c_str(), "READ");
    RooFitResult * result = (RooFitResult*)file->Get("fit_result");

    // Loop through results and add
    RooArgList vars = result->floatParsFinal();
    RooRealVar * var;
    TIterator * it = vars.createIterator();
    while ((var = (RooRealVar*)it->Next())) {
        std::string fullname = var->GetName(); 
        std::string varname = fullname.substr(fullname.find("_params_") + 8);
        if (CheckForExistence(varname)) {
            if (warnings) {
                std::cout << "Parameter " << varname << " already exists!" 
                    << " Overwriting with value from RooFitResult." 
                    << std::endl;
            }
            ((RooRealVar*)m_map[varname])->setVal(var->getVal());
            ((RooRealVar*)m_map[varname])->setError(var->getError());
        } else {
            AddItem(varname, var);
        }
    }
}


// =================================================================
// Adjust the value of a parameter by sigma around its initial value
// =================================================================
void ParameterManager::AdjustValue(std::string name, double sigma, bool force_pos) {

    // Check it exists
    if (!CheckForExistence(name)) {
        std::cout << "Parameter " << name << " not found! Cannot shift value."
            << std::endl;
    }

    // Get new value
    double mean = ((RooRealVar*)Get(name))->getVal();
    double value = -1;
    if (force_pos) {
        while (value < 0) {
            gRandom->SetSeed(0);
            value = gRandom->Gaus(mean, sigma);
            std::cout << "Shifted " << name << " from " << mean << " to " <<  value << std::endl;
            std::cout << "(sigma = " << sigma << ")" << std::endl;
        }
    } else {
        gRandom->SetSeed(0);
        value = gRandom->Gaus(mean, sigma);
        std::cout << "Shifted " << name << " from " << mean << " to " <<  value << std::endl;
        std::cout << "(sigma = " << sigma << ")" << std::endl;
    }

    // Set new value
    ((RooRealVar*)Get(name))->setVal(value);
    std::cout << "Adjusted parameter " << name << " from " << mean << " to " <<
        value << std::endl;
}


// ===========================
// Change value of a parameter
// ===========================
void ParameterManager::ChangeValue(std::string name, double newval) {

    // Check it exists
    if (!CheckForExistence(name)) {
        std::cout << "Warning: " << name << " not found! Cannot change value."
            << std::endl;
        return;
    }

    std::cout <<  "Changing value of " << name << " to " << newval << std::endl;
    // Set new value
    RooRealVar * var = (RooRealVar*)Get(name);
    var->setVal(newval);
    std::cout << "Success" << std::endl;
}


// =====================================
// Change value and range of a parameter
// =====================================
void ParameterManager::ChangeValue(std::string name, double newval,
        double min, double max) {

    // Check it exists
    if (!CheckForExistence(name)) {
        std::cout << "Warning: Parameter " << name << " not found! Cannot change value."
            << std::endl;
        return;
    }

    // Change range
    RooRealVar * var = (RooRealVar*)Get(name);
    var->setRange(min, max);
    var->setVal(newval);
}


// ===========================
// Change error of a parameter
void ParameterManager::ChangeError(std::string name, double error) {

    // Check it exists
    if (!CheckForExistence(name)) {
        std::cout << "Warning: parameter " << name << " not found! Cannot change value."
            << std::endl;
        return;
    }
    // Set new value
    RooRealVar * var = (RooRealVar*)Get(name);
    var->Print();
    var->setError(error);
    var->Print();
    std::cout << "\n";
}


// ==============================
// Set a parameter to be constant
// ==============================
void ParameterManager::SetConstant(std::string name) {

    // Check it exists
    if (!CheckForExistence(name)) {
        std::cout << "Parameter " << name << " not found! Cannot set constant."
            << std::endl;
    }
    // Set new value
    RooRealVar * var = (RooRealVar*)Get(name);
    var->setConstant();
}


// ========================
// Get value of a parameter
// ========================
double ParameterManager::GetValue(std::string name) {
    RooRealVar * var = (RooRealVar*)Get(name);
    return var->getVal();
}


// ========================
// Get error of a parameter
// ========================
double ParameterManager::GetError(std::string name) {
    RooRealVar * var = (RooRealVar*)Get(name);
    return var->getError();
}


// ==============================
// Print values of each parameter
// ==============================
void ParameterManager::PrintValues() {
    for (auto item : m_map) {
        // Don't print value of blind variables
        if (std::find(m_blindVars.begin(), m_blindVars.end(), item.first) != 
                m_blindVars.end()) {
            std::cout << item.first << ": ???" << std::endl;
        } else {
            std::cout << item.first << ": " << ((RooRealVar*)item.second)->getVal()
                << std::endl;
        }
    }
}


// ==============================
// Print values of each parameter
// ==============================
void ParameterManager::PrintValues(std::string filename) {
    std::ofstream file(filename);
    for (auto item : m_map) {
        // Don't print value of blind variables
        if (std::find(m_blindVars.begin(), m_blindVars.end(), item.first) != 
                m_blindVars.end()) {
            file << item.first << ": ???" << std::endl;
        } else {
            file << item.first << ": " << ((RooRealVar*)item.second)->getVal()
                << std::endl;
        }
    }
    file.close();
}
