#ifndef TWO_AND_FOUR_BODY_BASE_HPP
#define TWO_AND_FOUR_BODY_BASE_HPP

#include "TwoBodyBase.hpp"

// ================================
// Base class for a two-body fitter
// ================================
class TwoAndFourBodyBase : public TwoBodyBase {

public:
    TwoAndFourBodyBase(std::string name, RooRealVar * x, RooCategory * cat);
    virtual ~TwoAndFourBodyBase() {};

private:
    // Parameter setup
    void SetConstantParameters();
    void SetDependentParameters();

    // Shape setup
    void MakeComponentShapes();
    void MakeModeShapes();

    bool m_split;

};

#endif // TWO_AND_FOUR_BODY_BASE_HPP
