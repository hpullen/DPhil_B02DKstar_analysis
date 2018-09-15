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

protected:
    // Parameter setup
    virtual void SetConstantParameters();
    virtual void SetDependentParameters();

    // Shape setup
    virtual void MakeComponentShapes();
    virtual void MakeModeShapes();

    // Check whether shapes are split into plus and minus
    bool IsSplit();
};

#endif // TWO_BODY_BASE_HPP
