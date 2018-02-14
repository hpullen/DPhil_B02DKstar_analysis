#ifndef SHAPE_MANAGER_HPP
#define SHAPE_MANAGER_HPP

#include "ManagerBase.hpp"

class RooRealVar;
class ParameterManager;

// =============================
// Class for storing RooFit PDFs
// =============================
class ShapeManager : public ManagerBase {

public:
    ShapeManager(std::string name, RooRealVar * x, ParameterManager * pars);
    ~ShapeManager();

    // Methods for creating PDFs
    void AddCrystalBall(std::string name, std::string mean, std::string sigma,
            std::string alpha, std::string n);
    void AddExponential(std::string name, std::string slope);

    // Methods for adding PDFs
    void CombineShapes(std::string name, std::string shape1, std::string shape2,
            std::string fraction);

private:
    

private:
    // Fit variable w.r.t which all shapes are made
    RooRealVar * m_x;

    // Associated parameter manager from which to get shape parameters
    ParameterManager * m_pars;

};

#endif // SHAPE_MANAGER_HPP
