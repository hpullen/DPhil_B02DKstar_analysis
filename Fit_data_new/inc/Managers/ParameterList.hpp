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
    ~ParameterList();

    std::vector<std::string> GetVector();
    RooArgList GetArgList(const std::map<std::string, RooAbsReal*> & map);

private:
    int m_nPars;
    std::string m_par1;
    std::string m_par2;
    std::string m_par3;
    std::string m_par4;

};

#endif // PARAMETER_LIST_HPP
