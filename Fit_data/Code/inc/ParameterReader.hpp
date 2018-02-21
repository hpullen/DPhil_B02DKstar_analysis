#ifndef PARAMETER_READER_H
#define PARAMETER_READER_H

#include <map>
#include <string>

// ============================================
// Class for reading fit parameters from a file
// ============================================
class ParameterReader {

public:
    ParameterReader();
    ~ParameterReader();

    double operator[](std::string name);
    void readParams(std::string name, std::string path);
    
private:
    double at(std::string name);

    std::map<std::string, double> m_params;

};

#endif // PARAMETER_READER_H
