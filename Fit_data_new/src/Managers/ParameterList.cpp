#include "RooAbsReal.h"

#include "ParameterList.hpp"


// ============================
// Constructor with 1 parameter
// ============================
ParameterList::ParameterList(std::string par1) : 
    m_nPars(1), 
    m_par1(par1) {}


// =============================
// Constructor with 2 parameters
// =============================
ParameterList::ParameterList(std::string par1, std::string par2) : 
    m_nPars(2), 
    m_par1(par1), 
    m_par2(par2) {}


// =============================
// Constructor with 3 parameters
// =============================
ParameterList::ParameterList(std::string par1, std::string par2, std::string par3) : 
    m_nPars(3), 
    m_par1(par1), 
    m_par2(par2),
    m_par3(par3) {}


// =============================
// Constructor with 4 parameters
// =============================
ParameterList::ParameterList(std::string par1, std::string par2, std::string par3,
        std::string par4) : 
    m_nPars(4), 
    m_par1(par1), 
    m_par2(par2),
    m_par3(par3),
    m_par4(par4) {}


// ==========
// Destructor
// ==========
ParameterList::~ParameterList() {}


// =========================================
// Return a vector containing the parameters
// =========================================
std::vector<std::string> ParameterList::GetVector() {
    std::vector<std::string> param_vector;
    param_vector.push_back(m_par1);
    if (m_nPars > 1) {
        param_vector.push_back(m_par2);
    }
    if (m_nPars > 2) {
        param_vector.push_back(m_par3);
    }
    if (m_nPars > 3) {
        param_vector.push_back(m_par4);
    }
    return param_vector;
}


// ============================================================================
// Return a RooArgList containing the RooAbsReals mapped to the parameter names
// ============================================================================
RooArgList ParameterList::GetArgList(const std::map<std::string, RooAbsReal*> & map) 
{
    switch (m_nPars) {
        case 1 : return RooArgList(*map.at(m_par1));
        case 2 : return RooArgList(*map.at(m_par1), *map.at(m_par2));
        case 3 : return RooArgList(*map.at(m_par1), *map.at(m_par2), 
                         *map.at(m_par3));
        case 4 : return RooArgList(*map.at(m_par1), *map.at(m_par2), 
                         *map.at(m_par3), *map.at(m_par4));
        default : return RooArgList();
    }
}
