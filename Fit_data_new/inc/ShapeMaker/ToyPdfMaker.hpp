#ifndef TOY_PDF_MAKER_HPP
#define TOY_PDF_MAKER_HPP

#include "DataPdfMaker.hpp"

// ===============================================
// Class for making a simple PDF to generate toys 
// ===============================================
class ToyPdfMaker : public DataPdfMaker {

public:
    ToyPdfMaker(RooRealVar * x, RooCategory * cat, std::string input_file);
    ToyPdfMaker(std::string name, RooRealVar * x, RooCategory * cat,
            std::string input_file);
    ~ToyPdfMaker();

private:
    void MakeComponents();
    std::string m_inputfile;
};


#endif // TOY_PDF_MAKER_HPP
