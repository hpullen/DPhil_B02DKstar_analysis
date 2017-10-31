#include <fstream>
#include <iostream>

#include "RangeReader.hpp"

// ===================
// Default constructor
// ===================
RangeReader::RangeReader() {
    m_range_read = false;
}


// =============================
// Constructor with file reading
// =============================
RangeReader::RangeReader(std::string filename) {
    readRange(filename);
}


// ==========
// Destructor
// ==========
RangeReader::~RangeReader() {
}


// ================================
// Read in range values from a file
// ================================
void RangeReader::readRange(std::string filename) {
    std::ifstream file(filename);

    if (file.good()) {
        std::string word;
        file >> word;
        m_low = std::stod(word);
        file >> word;
        m_high = std::stod(word);
        m_range_read = true;
    } else {
        std::cout << "No file found at: " << filename << std::endl;
        m_range_read = false;
    }
}


// =====================
// Get minimum of range 
// =====================
double RangeReader::getLow() {
    if (m_range_read) {
        return m_low;
    } else {
        std::cout << "Warning: no range file has been read! Using default value." 
            << std::endl;
        return m_default_low;
    }
}


// =====================
// Get maximum of range 
// =====================
double RangeReader::getHigh() {
    if (m_range_read) {
        return m_high;
    } else {
        std::cout << "Warning: no range file has been read! Using default value." 
            << std::endl;
        return m_default_high;
    }
}
