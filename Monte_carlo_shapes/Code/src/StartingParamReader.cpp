#include <fstream>
#include <iostream>

#include "StartingParamReader.hpp"


// ===================
// Default constructor
// ===================
StartingParamReader::StartingParamReader() {
    m_params_read = false;
}


// ==================================
// Constructor with parameter reading
// ==================================
StartingParamReader::StartingParamReader(std::string filename) {
    readParams(filename);
}


// ==========
// Destructor
// ==========
StartingParamReader::~StartingParamReader() {
}


// ============================
// Read in parameters from file
// ============================
void StartingParamReader::readParams(std::string filename) {
    std::ifstream file(filename);

    if (!file.good()) {
        std::cout << "Could not find param file at: " << filename << std::endl;
        m_params_read = false;
        return;
    }

    std::string word;
    file >> word;
    while (!file.eof()) {
        std::string name = word;
        file >> word;
        double start = std::stod(word);
        file >> word;
        double low = std::stod(word);
        file >> word;
        double high = std::stod(word);
        file >> word;
        m_params.emplace(name, StartingParam(start, low, high));
    }
    m_params_read = true;
}


// ==================
// Return a parameter
// ==================
StartingParam StartingParamReader::getParam(std::string name) {
    if (!m_params_read) {
        std::cout << "Warning: no parameters have been read! Returning zero." << 
            std::endl;
        return StartingParam(0, 0, 0);
    }

    return m_params.at(name);
}
