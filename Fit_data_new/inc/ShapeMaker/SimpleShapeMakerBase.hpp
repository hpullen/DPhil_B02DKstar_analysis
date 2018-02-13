#ifndef SIMPLE_SHAPE_MAKER_BASE_HPP
#define SIMPLE_SHAPE_MAKER_BASE_HPP

#include "ShapeMakerBase.hpp"

// ======================================================
// Base class for a simple fit with only three components
// ======================================================
class SimpleShapeMakerBase : public ShapeMakerBase {

public:
    SimpleShapeMakerBase(std::string name);
    virtual ~SimpleShapeMakerBase() {};

private:
    // Parameter setup
    void SetConstantParameters();
    void SetDependentParameters();

    // Shape setup
    void MakeComponentShapes();
    void MakeModeShapes();
    void MakeSimultaneousShape();

};

#endif // SIMPLE_SHAPE_MAKER_BASE_HPP
