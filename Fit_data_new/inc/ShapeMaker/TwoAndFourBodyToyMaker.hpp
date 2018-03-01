#ifndef TWO_AND_FOUR_BODY_TOY_MAKER_HPP
#define TWO_AND_FOUR_BODY_TOY_MAKER_HPP

#include "TwoAndFourBodyBase.hpp"

// ===============================================
// Class for making a simple PDF to generate toys 
// ===============================================
class TwoAndFourBodyToyMaker : public TwoAndFourBodyBase {

public:
    TwoAndFourBodyToyMaker(RooRealVar * x, RooCategory * cat, std::string input_file);
    TwoAndFourBodyToyMaker(std::string name, RooRealVar * x, RooCategory * cat,
            std::string input_file);
    ~TwoAndFourBodyToyMaker();

private:
    void SetFloatingParameters();

    std::string m_inputfile;
};


#endif // TWO_AND_FOUR_BODY_TOY_MAKER_HPP
