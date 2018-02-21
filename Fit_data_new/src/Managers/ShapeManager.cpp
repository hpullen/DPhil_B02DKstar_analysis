#include "RooAddPdf.h"
#include "RooCBShape.h"
#include "RooExponential.h"
#include "RooRealVar.h"

#include "RooHILLdini.h"
#include "RooHORNSdini.h"
#include "RooLITTLEHORNSdini.h"

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


// ==================
// Add a RooHORNSdini
// ==================
void ShapeManager::AddHorns(std::string name, std::string a, std::string b,
        std::string csi, std::string shift, std::string sigma,
        std::string ratio, std::string frac) {
    RooHORNSdini * shape = new RooHORNSdini((m_name + "_" + name).c_str(), "", *m_x, 
            *m_pars->Get(a), *m_pars->Get(b), *m_pars->Get(csi), 
            *m_pars->Get(shift), *m_pars->Get(sigma), 
            *m_pars->Get(ratio), *m_pars->Get(frac));
    AddItem(name, shape);
}


// =================
// Add a RooHILLdini
// =================
void ShapeManager::AddHill(std::string name, std::string a, std::string b,
        std::string csi, std::string shift, std::string sigma,
        std::string ratio, std::string frac) {
    RooHILLdini * shape = new RooHILLdini((m_name + "_" + name).c_str(), "", *m_x, 
            *m_pars->Get(a), *m_pars->Get(b), *m_pars->Get(csi), 
            *m_pars->Get(shift), *m_pars->Get(sigma), *m_pars->Get(ratio), 
            *m_pars->Get(frac));
    AddItem(name, shape);
}


// ========================
// Add a RooLITLLEHORNSdini
// ========================
void ShapeManager::AddLittleHorns(std::string name, std::string a, std::string b,
        std::string csi, std::string shift, std::string sigma,
        std::string ratio, std::string frac, std::string shiftg) {
    RooLITTLEHORNSdini * shape = new RooLITTLEHORNSdini((m_name + "_" + name).c_str(), 
            "", *m_x, *m_pars->Get(a), *m_pars->Get(b), *m_pars->Get(csi), 
            *m_pars->Get(shift), *m_pars->Get(sigma), *m_pars->Get(ratio), 
            *m_pars->Get(frac), *m_pars->Get(shiftg));
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
