#include <fstream>
#include <iostream>

#include "ParameterReader.hpp"


// ===========
// Constructor
// ===========
ParameterReader::ParameterReader() {}


// ==========
// Destructor
// ==========
ParameterReader::~ParameterReader() {}


// ====================
// Retrieve a parameter
// ====================
double ParameterReader::operator[](std::string name) {
    return getParam(name);
}


// ==============================
// Read in parameters from a file
// ==============================
void ParameterReader::readParams(std::string name, std::string path) {
    
    // Open the file
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cout << "Parameter file " << path << " not found!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Loop through and read parameters
    std::string param_name = "";
    std::string param_val = "";
    std::string param_error = "";
    file >> param_name;
    int count;
    while (!file.eof()) {
        file >> param_val;
        file >> param_error;
        m_params[name + "_" + param_name] = std::stod(param_val);
        count++;
        file >> param_name;
    }
    std::cout << "Read " << count << " parameters from file " << path << std::endl;
    file.close();
}




// ===============================
// Return the value of a parameter
// ===============================
double ParameterReader::getParam(std::string name) {
    if (m_params.find(name) == m_params.end()) {
        std::cout << "Parameter " << name << " not found in parameter reader!" 
            << std::endl;
        return -1;
    }  else {
        return m_params[name];
    }
}
