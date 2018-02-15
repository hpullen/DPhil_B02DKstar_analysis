#include "RooAddPdf.h"
#include "RooCBShape.h"
#include "RooExponential.h"
#include "RooRealVar.h"

#include "ParameterManager.hpp"
#include "ShapeManager.hpp"

// ===========
// Constructor
// ===========
ShapeManager::ShapeManager(std::string name, RooRealVar * x, 
        ParameterManager * pars) : ManagerBase(name), m_x(x), m_pars(pars) {}


// ==========
// Destructor
// ==========
ShapeManager::~ShapeManager() {}


// ========================
// Add a crystal ball shape
// ========================
void ShapeManager::AddCrystalBall(std::string name, std::string mean, 
        std::string sigma, std::string alpha, std::string n) {
    RooCBShape * shape = new RooCBShape((m_name + "_" + name).c_str(), "", *m_x,
            *m_pars->Get(mean), *m_pars->Get(sigma), *m_pars->Get(alpha),
            *m_pars->Get(n));
    AddItem(name, shape);
}


// ========================
// Add an exponential shape
// ========================
void ShapeManager::AddExponential(std::string name, std::string slope) {
    RooExponential * shape = new RooExponential((m_name + "_" + name).c_str(), "", 
            *m_x, *m_pars->Get(slope));
    AddItem(name, shape);
}


// =============================
// Combine two pre-existing PDFs
// =============================
void ShapeManager::CombineShapes(std::string name, std::string shape1, 
        std::string shape2, std::string frac) {

    // Make RooAddPdf
    RooAddPdf * shape = new RooAddPdf((m_name + "_" + name).c_str(), "", 
            RooArgList(*Get(shape1), *Get(shape2)), 
            RooArgList(*m_pars->Get(frac)));
    AddItem(name, shape);

}


// =====================
// Combine a map of PDFs
// =====================
void ShapeManager::CombineShapes(std::string name, std::map<std::string,
        std::string> shapes_and_coefs) {

    // Make lists of shapes and coefficients
    RooArgList pdfList;
    RooArgList coefList;
    for (auto item : shapes_and_coefs) {
        pdfList.add(*Get(item.first));
        coefList.add(*m_pars->Get(item.second));
    }

    // Make RooAddPdf
    RooAddPdf * shape = new RooAddPdf((m_name + "_" + name).c_str(), "",
            pdfList, coefList);
    AddItem(name, shape);

}
