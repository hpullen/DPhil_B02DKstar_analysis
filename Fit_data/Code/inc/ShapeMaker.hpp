#include <string>

// Forward declarations
class ParameterReader;
class RooAbsPdf;
class RooAbsReal;
class RooCategory;
class RooRealVar;
class RooSimultaneous;
class RooDataSet;

// ========================================
// Class for creating PDFs for toys or data
// ========================================
class ShapeMaker {

public:
    ShapeMaker(std::string sum, RooRealVar * Bd_M);
    ~ShapeMaker();

    RooSimultaneous * makeFitPdf();
    RooSimultaneous * makeGenerationPdf(std::string results_file);
    void saveFitHistograms(std::string filename,
            std::map<std::string, RooDataSet*> dataMap);
    RooCategory * getCategory();

private:
    RooSimultaneous * makePdf();

    std::map<std::string, RooAbsReal*> m_vars;
    std::map<std::string, RooAbsReal*> m_gen_vars;
    std::map<std::string, RooAbsPdf*> m_pdfs;
    std::vector<std::string> m_modes;
    RooCategory * m_cat;
    std::string m_sum;
    RooRealVar * m_x;
    ParameterReader * m_pr;

};
