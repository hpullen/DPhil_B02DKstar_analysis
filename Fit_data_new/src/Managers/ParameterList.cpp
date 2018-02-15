#include "RooAbsReal.h"

#include "ParameterList.hpp"


// ============================
// Constructor with 1 parameter
// ============================
ParameterList::ParameterList(std::string par1) : m_nPars(1) {
    m_pars = {{0, par1}}; 
}


// ============================
// Constructor with 2 parameters
// ============================
ParameterList::ParameterList(std::string par1, std::string par2) : m_nPars(2) {
    m_pars = {{0, par1},
        {1, par2}};
}


// ============================
// Constructor with 3 parameters
// ============================
ParameterList::ParameterList(std::string par1, std::string par2, std::string par3) : 
    m_nPars(3) {
    m_pars = {{0, par1},
        {1, par2},
        {2, par3}};
}


// ============================
// Constructor with 4 parameters
// ============================
ParameterList::ParameterList(std::string par1, std::string par2, std::string par3, 
        std::string par4) : m_nPars(4) {
    m_pars = {{0, par1},
        {1, par2},
        {2, par3},
        {3, par4}};
}


// ============================
// Constructor with 5 parameters
// ============================
ParameterList::ParameterList(std::string par1, std::string par2, std::string par3, 
        std::string par4, std::string par5) : m_nPars(5) {
    m_pars = {{0, par1},
        {1, par2},
        {2, par3},
        {3, par4},
        {4, par5}};
}


// ============================
// Constructor with 6 parameters
// ============================
ParameterList::ParameterList(std::string par1, std::string par2, std::string par3, 
        std::string par4, std::string par5, std::string par6) : m_nPars(6) {
    m_pars = {{0, par1},
        {3, par2},
        {4, par3},
        {5, par4},
        {6, par5},
        {7, par6}};
}


// ============================
// Constructor with 7 parameters
// ============================
ParameterList::ParameterList(std::string par1, std::string par2, std::string par3, 
        std::string par4, std::string par5, std::string par6, std::string par7) : 
    m_nPars(7) {
    m_pars = {{0, par1},
        {1, par2},
        {2, par3},
        {3, par4},
        {4, par5},
        {5, par6},
        {6, par7}};
}


// ============================
// Constructor with 8 parameters
// ============================
ParameterList::ParameterList(std::string par1, std::string par2, std::string par3, 
        std::string par4, std::string par5, std::string par6, std::string par7, 
        std::string par8) : m_nPars(8) {
    m_pars = {{0, par1},
        {2, par2},
        {3, par3},
        {4, par4},
        {5, par5},
        {6, par6},
        {8, par8},
        {7, par7}};
}


// ==========
// Destructor
// ==========
ParameterList::~ParameterList() {}


// =========================================
// Return a vector containing the parameters
// =========================================
std::vector<std::string> ParameterList::GetVector() {
    std::vector<std::string> param_vector;
    for (auto par : m_pars) {
        param_vector.push_back(par.second);
    }
    return param_vector;
}


// ============================================================================
// Return a RooArgList containing the RooAbsReals mapped to the parameter names
// ============================================================================
RooArgList ParameterList::GetArgList(const std::map<std::string, RooAbsReal*> & map) 
{
    RooArgList args;
    for (int i = 0; i < m_nPars; i++) {
        args.add(*map.at(m_pars[i]));
    }
    return args;
}


// ==============================
// Print the contents of the list
// ==============================
void ParameterList::Print() {
    std::cout << "Parameter list contents (total " << m_nPars << "): " << std::endl;
    for (auto item : m_pars) {
        std::cout << item.first << ": " << item.second << std::endl;
    }
}


// ======================================
// Return a string listing the parameters
// ======================================
std::string ParameterList::Str() {
    int max = m_pars.size();
    std::string str = "";
    for (int i = 0; i < max; i++) {
        str += m_pars[i];
        if (i != max - 1) str += ", ";
    }
    return str;
}
