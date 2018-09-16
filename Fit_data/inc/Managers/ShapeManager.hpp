#ifndef SHAPE_MANAGER_HPP
#define SHAPE_MANAGER_HPP

#include "ManagerBase.hpp"

class RooRealVar;
class ParameterManager;
class RooArgSet;

// =============================
// Class for storing RooFit PDFs
// =============================
class ShapeManager : public ManagerBase {

public:
    ShapeManager(std::string name, RooRealVar * x, ParameterManager * pars);
    ~ShapeManager();

    // Methods for creating PDFs
    void AddGaussian(std::string name, std::string mean, std::string sigma);
    void AddCrystalBall(std::string name, std::string mean, std::string sigma,
            std::string alpha, std::string n);
    void AddExponential(std::string name, std::string slope);
    void AddHill(std::string name, std::string a, std::string b, 
            std::string csi, std::string shift, std::string sigma,
            std::string ratio, std::string frac);
    void AddHorns(std::string name, std::string a, std::string b, 
            std::string csi, std::string shift, std::string sigma,
            std::string ratio, std::string frac);
    void AddLittleHorns(std::string name, std::string a, std::string b, 
            std::string csi, std::string shift, std::string sigma,
            std::string ratio, std::string frac, std::string shiftg);
    void AddConstraint(std::string param, double mean, double width);

    // Methods for adding PDFs
    void CombineShapes(std::string name, std::string shape1, std::string shape2,
            std::string fraction);
    void CombineShapes(std::string name, std::map<std::string, std::string> 
            shapes_and_coefs);

    // Get RooArgList of constraint PDFs
    RooArgSet * GetConstraintPdfs();
    bool HasConstraints();

private:
    

private:
    // Fit variable w.r.t which all shapes are made
    RooRealVar * m_x;

    // Associated parameter manager from which to get shape parameters
    ParameterManager * m_pars;

    // Constraint PDFs
    std::map<std::string, std::pair<double, double>> m_constraints;

};

#endif // SHAPE_MANAGER_HPP