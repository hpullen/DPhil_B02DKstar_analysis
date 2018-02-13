#ifndef SIMPLE_TOY_MAKER_HPP
#define SIMPLE_TOY_MAKER_HPP

#include "SimpleShapeMakerBase.hpp"

// ===============================================
// Class for making a simple PDF to generate toys 
// ===============================================
class SimpleToyMaker : public SimpleShapeMakerBase {

public:
    SimpleToyMaker();
    SimpleToyMaker(std::string name);
    ~SimpleToyMaker();

private:
    void SetFloatingParameters();

};


#endif // SIMPLE_TOY_MAKER_HPP
