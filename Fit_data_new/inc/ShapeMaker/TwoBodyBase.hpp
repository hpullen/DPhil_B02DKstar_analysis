#ifndef TWO_BODY_BASE_HPP
#define TWO_BODY_BASE_HPP

#include "ShapeMakerBase.hpp"

// ================================
// Base class for a two-body fitter
// ================================
class TwoBodyBase : public ShapeMakerBase {

public:
    TwoBodyBase(std::string name, RooRealVar * x, RooCategory * cat);
    virtual ~TwoBodyBase() {};

private:
    // Parameter setup
    void SetConstantParameters();
    void SetDependentParameters();

    // Shape setup
    void MakeComponentShapes();
    void MakeModeShapes();

};

#endif // TWO_BODY_BASE_HPP
