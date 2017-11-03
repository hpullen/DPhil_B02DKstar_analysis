#include <iostream>

#include "TIterator.h"
#include "TFile.h"

#include "RooGaussian.h"
#include "RooCBShape.h"
#include "RooExponential.h"
#include "RooAddPdf.h"

#include "RooHORNSdini.h"
#include "RooHILLdini.h"
#include "RooLITTLEHORNSdini.h"
#include "RooWorkspace.h"
#include "RooArgSet.h"

#include "ParameterManager.hpp"
#include "ShapeManager.hpp"


// ===========
// Constructor
// ===========
ShapeManager::ShapeManager(std::string name) : m_name(name) {}


// ==========
// Destructor
// ==========
ShapeManager::~ShapeManager() {}


// ==============
// Return a shape
// ==============
RooAbsPdf * ShapeManager::getShape(std::string name) {
    
    // Warn if shape doesn't exist
    if (!m_shapes.count(name)) {
        std::cout << "Warning: Shape " << name << " does not exist in shape manager " 
            << m_name << "!" << std::endl;
    }

    // Return shape
    return m_shapes[name];
}


// =====================
// Return shape using []
// =====================
RooAbsPdf * ShapeManager::operator[](std::string name) {
    return getShape(name);
}


// ==================
// Add Gaussian shape
// ==================
void ShapeManager::addGaussian(std::string name, ParameterManager * params,
        std::string mean, std::string width) {

    checkForDuplicates(name);

    RooGaussian * gaussian = new RooGaussian((m_name + "_" + name).c_str(), "", 
            *params->getParam("x"), *params->getParam(mean), 
            *params->getParam(width));
    m_shapes.emplace(name, gaussian);
}


// ==================
// Add RooHORNS shape
// ==================
void ShapeManager::addHorns(std::string name, ParameterManager * params, std::string x,
        std::string a, std::string b, std::string csi, std::string shift, 
        std::string sigma, std::string ratio, std::string f) {

    checkForDuplicates(name);

    RooHORNSdini * horns = new RooHORNSdini((m_name + "_" + name).c_str(), "", 
            *params->getParam(x), *params->getParam(a), *params->getParam(b),
            *params->getParam(csi), *params->getParam(shift),
            *params->getParam(sigma), *params->getParam(ratio),
            *params->getParam(f));
    m_shapes.emplace(name, horns);
}


// =================
// Add RooHILL shape
// =================
void ShapeManager::addHill(std::string name, ParameterManager * params, std::string x,
        std::string a, std::string b, std::string csi, std::string shift, 
        std::string sigma, std::string ratio, std::string f) {

    checkForDuplicates(name);

    RooHILLdini * hill = new RooHILLdini((m_name + "_" + name).c_str(), "", 
            *params->getParam(x), *params->getParam(a), *params->getParam(b),
            *params->getParam(csi), *params->getParam(shift),
            *params->getParam(sigma), *params->getParam(ratio),
            *params->getParam(f));
    m_shapes.emplace(name, hill);
}


// ========================
// Add RooLITTLEHORNS shape
// ========================
void ShapeManager::addLittleHorns(std::string name, ParameterManager * params, std::string x,
        std::string a, std::string b, std::string csi, std::string shift, 
        std::string sigma, std::string ratio, std::string f, std::string shiftg) {

    checkForDuplicates(name);

    RooLITTLEHORNSdini * littlehorns = new RooLITTLEHORNSdini(
            (m_name + "_" + name).c_str(), "", 
            *params->getParam(x), *params->getParam(a), *params->getParam(b),
            *params->getParam(csi), *params->getParam(shift),
            *params->getParam(sigma), *params->getParam(ratio),
            *params->getParam(f), *params->getParam(shiftg));
    m_shapes.emplace(name, littlehorns);
}


// ======================
// Add crystal ball shape
// ======================
void ShapeManager::addCrystalBall(std::string name, ParameterManager * params, std::string x,
        std::string mean, std::string sigma, std::string alpha, std::string n) {

    checkForDuplicates(name);

    RooCBShape * cb = new RooCBShape((m_name + "_" + name).c_str(), "", 
            *params->getParam(x),
            *params->getParam(mean), *params->getParam(sigma),
            *params->getParam(alpha), *params->getParam(n));
    m_shapes.emplace(name, cb);
}


// =====================
// Add exponential shape
// =====================
void ShapeManager::addExponential(std::string name, ParameterManager * params,
        std::string x, std::string slope) {

    checkForDuplicates(name);

    RooExponential * expo = new RooExponential((m_name + "_" + name).c_str(), "", 
            *params->getParam(x), *params->getParam(slope));
    m_shapes.emplace(name, expo);
}


// ===============================
// Add a PDF made up of other PDFs
// ===============================
void ShapeManager::addAddPdf(std::string name, ParameterManager * params, 
        ShapeManager * shapes, std::map<std::string, std::string> shapes_yields) {

    checkForDuplicates(name);

    // Make lists of shapes and yields
    RooArgList shapes_list;
    RooArgList yields_list;
    for (auto item : shapes_yields) {
        shapes_list.add(*shapes->getShape(item.first));
        if (item.second != "") {
            yields_list.add(*params->getParam(item.second));
        }
    }

    // Make shape
    RooAddPdf * add_pdf = new RooAddPdf((m_name + "_" + name).c_str(), "", shapes_list, 
            yields_list);
    m_shapes.emplace(name, add_pdf);
}



// ==============================================
// Check whether named ROOT object already exists
// ==============================================
void ShapeManager::checkForDuplicates(std::string name) {
    if (m_shapes.count(name)) {
        std::cout << "Warning: shape with name " << name << " already exists in "
            << m_name << std::endl;
    }
}


void ShapeManager::reset() {
    m_shapes.clear();
}
