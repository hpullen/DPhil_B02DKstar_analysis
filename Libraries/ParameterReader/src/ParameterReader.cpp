#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "ParameterReader.hpp"


// ===========
// Constructor
// ===========
ParameterReader::ParameterReader() : m_base("") {}


// ======================================
// Constructor with custom base directory
// ======================================
ParameterReader::ParameterReader(std::string base_dir) : m_base(base_dir) {}


// ==========
// Destructor
// ==========
ParameterReader::~ParameterReader() {}


// ====================================
// Read a set of parameters from a file
// ====================================
void ParameterReader::ReadParameters(std::string filename, std::string set_name) {

    // Warn if overwriting
    if (SetExists(set_name)) {
        std::cout << "ParameterManager: Warning! overwriting existing set " << 
            set_name << std::endl;
    }

    // Vector to store parameters
    std::map<std::string, Parameter*> pars;

    // Open file
    std::string total_file = (m_base == "") ? filename : m_base + "/" + filename;
    std::ifstream file(total_file);
    if (!file.is_open()) {
        std::cout << "Parameter file " << m_base + "/" + filename << 
            " not found!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Loop through lines and read parameters
    std::string line;
    int line_count = 0;
    while (std::getline(file, line)) {

        // Set up
        line_count++;
        Parameter * par = new Parameter;
        std::string par_name;
        std::stringstream ss(line);

        // Fill name and value strings
        if (!(ss >> par_name >> par->value)) {
            std::cout << "Error on line " << line_count << " of file " << m_base <<
                filename << "; wrong format?" << std::endl;
            exit (EXIT_FAILURE);
        }

        // Attempt to fill error string
        if (!(ss >> par->error)) {
            par->error = 0;
            par->has_error = false;
        } else {
            par->has_error = true;
        }

        // Add to vector
        pars.emplace(par_name, par);
    }

    // Fill map
    m_pars.emplace(set_name, pars);
}


// =========================
// Check if parameter exists
// =========================
bool ParameterReader::Contains(std::string param_name, std::string set_name) {
    return ParameterExists(param_name, set_name);
}


// ========================
// Get value of a parameter
// ========================
double ParameterReader::GetValue(std::string param_name, std::string set_name) {
    CheckParameter(param_name, set_name);
    return m_pars[set_name][param_name]->value;
}


// ========================
// Get error of a parameter
// ========================
double ParameterReader::GetError(std::string param_name, std::string set_name) {
    CheckParameter(param_name, set_name);
    if (m_pars[set_name][param_name]->has_error) {
        return m_pars[set_name][param_name]->error;
    } else {
        std::cout << "ParameterReader: Error! Parameter \"" << param_name << 
            "\" in set \"" << set_name << "\" has no associated error!" << std::endl;
        exit (EXIT_FAILURE);
    }
}


// ====================
// Print all parameters
// ====================
void ParameterReader::Print() {
    std::cout << std::endl;
    for (auto set_name : m_pars) {
        Print(set_name.first);
        std::cout << std::endl;
    }
}


// ==============================
// Print single set of parameters
// ==============================
void ParameterReader::Print(std::string set_name) {

    // Check that set exists
    if (!SetExists(set_name)) {
        std::cout << "ParameterReader: Error! Attempting to print non-existant set \""
            << set_name << "\"" << std::endl;
        return;
    }

    // Print
    std::cout << "ParameterReader: Parameters in set \"" << set_name << "\":" << 
        std::endl;
    for (auto par : m_pars[set_name]) {
        std::cout << "ParameterReader: " << par.first << " = " 
            << par.second->value;
        if (par.second->has_error) {
            std::cout << " +/- " << par.second->error;
        } 
        std::cout << std::endl;
    }
}


// ===========================================
// Check if a set of parameters already exists
// ===========================================
bool ParameterReader::SetExists(std::string set_name) {
    return (m_pars.find(set_name) != m_pars.end());
}


// ========================================
// Check if a parameter exists within a set
// ========================================
bool ParameterReader::ParameterExists(std::string param_name, std::string set_name) {
    if (!SetExists(set_name)) return false;
    return (m_pars[set_name].find(param_name) != m_pars[set_name].end());
}


// ==========================================
// Warn and exit if a parameter doesn't exist
// ==========================================
void ParameterReader::CheckParameter(std::string param_name, std::string set_name) {
    if (!SetExists(set_name)) {
        std::cout << "ParameterReader: Error! Parameter set " << set_name << 
            " does not exist!" << std::endl;
        exit (EXIT_FAILURE);
    } 
    if (!ParameterExists(param_name, set_name)) {
        std::cout << "ParameterReader: Error! Parameter " << param_name << 
            " does not exist in set " << set_name << "!" << std::endl;
        exit (EXIT_FAILURE);
    }
}
