#ifndef TOY_PDF_MAKER_HPP
#define TOY_PDF_MAKER_HPP

#include "DataPdfMaker.hpp"

// ===============================================
// Class for making a simple PDF to generate toys 
// ===============================================
class ToyPdfMaker : public DataPdfMaker {

public:
    ToyPdfMaker(RooRealVar * x, RooCategory * cat, std::string input_file);
    ToyPdfMaker(RooRealVar * x, RooCategory * cat, std::string input_file, 
            bool high_stats);
    ToyPdfMaker(std::string name, RooRealVar * x, RooCategory * cat,
            std::string input_file);
    ~ToyPdfMaker();

    void SetHighStats(bool high_stats = true);

private:
    void MakeComponents();
    void AdjustYields();
    void AdjustYield(std::string name, double scale);
    std::string m_inputfile;
    bool m_high_stats;
};


#endif // TOY_PDF_MAKER_HPP