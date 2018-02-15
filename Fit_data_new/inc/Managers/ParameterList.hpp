#ifndef PARAMETER_LIST_HPP
#define PARAMETER_LIST_HPP

#include <map>
#include <string>

#include "RooArgList.h"

class RooAbsReal;

// ====================================================
// Class for holding an ordered list of parameter names
// ====================================================
class ParameterList {

public:
    ParameterList(std::string par1);
    ParameterList(std::string par1, std::string par2);
    ParameterList(std::string par1, std::string par2, std::string par3);
    ParameterList(std::string par1, std::string par2, std::string par3, 
            std::string par4);
    ParameterList(std::string par1, std::string par2, std::string par3, 
            std::string par4, std::string par5);
    ParameterList(std::string par1, std::string par2, std::string par3, 
            std::string par4, std::string par5, std::string par6);
    ParameterList(std::string par1, std::string par2, std::string par3, 
            std::string par4, std::string par5, std::string par6, 
            std::string par7);
    ParameterList(std::string par1, std::string par2, std::string par3, 
            std::string par4, std::string par5, std::string par6, 
            std::string par7, std::string par8);
    ~ParameterList();

    std::vector<std::string> GetVector();
    RooArgList GetArgList(const std::map<std::string, RooAbsReal*> & map);
    void Print();
    std::string Str();

private:
    int m_nPars;
    std::map<int, std::string> m_pars;

};

#endif // PARAMETER_LIST_HPP
