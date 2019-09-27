#include "RooAddPdf.h"
#include "RooArgSet.h"
#include "RooCBShape.h"
#include "RooExponential.h"
#include "RooGaussian.h"
#include "RooRealVar.h"

#include "RooHILLdini.h"
#include "RooHORNSdini.h"
#include "RooHORNSdini_misID.h"
#include "RooLITTLEHORNSdini.h"
#include "RooCruijff.h"

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


// ==============
// Add a Gaussian
// ==============
void ShapeManager::AddGaussian(std::string name, std::string mean,  
        std::string sigma) {
    RooGaussian * shape = new RooGaussian((m_name + "_" + name).c_str(), "", 
            *m_x, *m_pars->Get(mean), *m_pars->Get(sigma));
    AddItem(name, shape);
}


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

// =========================
// Add a mis ID RooHORNSdini
// =========================
void ShapeManager::AddHornsMisID(std::string name, std::string a, std::string b,
        std::string csi, std::string m1, std::string s1, std::string m2,
        std::string s2, std::string m3, std::string s3, std::string m4,
        std::string s4, std::string f1, std::string f2, std::string f3) {
    RooHORNSdini_misID * shape = new RooHORNSdini_misID((m_name + "_" + name).c_str(), "", 
            *m_x, *m_pars->Get(a), *m_pars->Get(b), *m_pars->Get(csi), 
            *m_pars->Get(m1), *m_pars->Get(s1), *m_pars->Get(m2), *m_pars->Get(s2),
            *m_pars->Get(m3), *m_pars->Get(s1), *m_pars->Get(m4), *m_pars->Get(s2),
            *m_pars->Get(f1), *m_pars->Get(f2), *m_pars->Get(f3));
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


// ================
// Add a RooCruijff
// ================
void ShapeManager::AddCruijff(std::string name, std::string mean, std::string sigma_L, 
        std::string sigma_R, std::string alpha_L, std::string alpha_R) {
    RooCruijff * shape = new RooCruijff((m_name + "_" + name).c_str(),
            "", *m_x, *m_pars->Get(mean), *m_pars->Get(sigma_L), *m_pars->Get(sigma_R),
            *m_pars->Get(alpha_L), *m_pars->Get(alpha_R));
    AddItem(name, shape);
}


// ======================
// Add a Gauss constraint
// ======================
void ShapeManager::AddConstraint(std::string param, double mean, double width) {

    // Add to constraints list
    m_constraints[param] = std::make_pair(mean, width);
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
        if (item.second != "") {
            coefList.add(*m_pars->Get(item.second));
        }
    }

    // Make RooAddPdf
    RooAddPdf * shape = new RooAddPdf((m_name + "_" + name).c_str(), "",
            pdfList, coefList);
    AddItem(name, shape);

}


// ================================
// Get RooArgSet of constraint PDFs
// ================================
RooArgSet * ShapeManager::GetConstraintPdfs() {

    // Check constraints exist
    if (!HasConstraints()) {
        std::cout << "ShapeManager::Error! No constraints present" << std::endl;
    }

    // Make RooArgset
    RooArgSet * args = new RooArgSet();
    std::cout << "Constraints: " << std::endl;

    // Loop through constraints
    for (auto pdf : m_constraints) {

        // Make variables for mean/width
        m_pars->AddRealVar(pdf.first + "_constraint_mean", pdf.second.first);
        m_pars->AddRealVar(pdf.first + "_constraint_width", pdf.second.second);

        // Make Gaussian
        RooGaussian * constraint = new RooGaussian((m_name + "_" + pdf.first + "_constraint").c_str(), "",
                *m_pars->Get(pdf.first), *m_pars->Get(pdf.first + "_constraint_mean"),
                *m_pars->Get(pdf.first + "_constraint_width"));
        AddItem(pdf.first + "_constraint", constraint);

        // Add to arg list
        args->add(*constraint);
        std::cout << pdf.first << std::endl;
    }
    std::cout << std::endl;
    return args;
}


// ====================================
// Return true if there are constraints
// ====================================
bool ShapeManager::HasConstraints() {
    return ! m_constraints.empty();
}
