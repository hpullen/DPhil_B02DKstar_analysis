#ifndef PARAMETER_READER_HPP
#define PARAMETER_READER_HPP

#include <map>
#include <string>

// ==================================================
// Class for reading lists of parameters from a files
// ==================================================
class ParameterReader {

public: 
    ParameterReader();
    ParameterReader(std::string base_dir);
    ~ParameterReader();

    void ReadParameters(std::string filename, std::string set_name = "default");
    bool Contains(std::string param_name, std::string set_name = "default");
    double GetValue(std::string param_name, std::string set_name = "default");
    double GetError(std::string param_name, std::string set_name = "default");
    void Print();
    void Print(std::string set_name);

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
    bool ParameterExists(std::string param_name, std::string set_name);
    void CheckParameter(std::string param_name, std::string set_name);

};

#endif // PARAMETER_READER_HPP
