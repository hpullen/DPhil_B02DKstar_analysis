#ifndef PARAMETER_READER_HPP
#define PARAMETER_READER_HPP

#include <map>
#include <vector>
#include <string>

// ==================================================
// Class for reading lists of parameters from a files
// ==================================================
class ParameterReader {

public: 
    ParameterReader();
    ParameterReader(std::string base_dir);
    ~ParameterReader();

    void ReadParameters(std::string set_name, std::string filename);
    double GetValue(std::string set_name, std::string param_name);
    double GetError(std::string set_name, std::string param_name);
    void Print();
    void Print(std::string set_name);
    std::vector<std::string> GetParameterList(std::string set_name);

private:
    // Struct for holding parameters
    struct Parameter {
        double value;
        double error;
        bool has_error;
    };

    // Map to store parameters
    std::string m_base;
    std::map<std::string, std::map<std::string, Parameter*>> m_pars;

    // Functions
    bool SetExists(std::string set_name);
    bool ParameterExists(std::string set_name, std::string param_name);
    void CheckParameter(std::string set_name, std::string param_name);
};

#endif // PARAMETER_READER_HPP
