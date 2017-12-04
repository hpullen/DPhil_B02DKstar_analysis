#include <string>

// Forward declarations
class ParameterReader;
class RooAbsPdf;
class RooAbsReal;
class RooCategory;
class RooRealVar;
class RooSimultaneous;
class RooDataSet;

// Map of strings and RooAbsReal
typedef std::map<std::string, RooAbsReal*> VarMap;
typedef std::map<std::string, RooAbsPdf*> PdfMap;

// ========================================
// Class for creating PDFs for toys or data
// ========================================
class ShapeMaker {

public:
    ShapeMaker(std::string sum, RooRealVar * Bd_M);
    ~ShapeMaker();

    RooSimultaneous * makeFitPdf(bool blind = true);
    RooSimultaneous * makeGenerationPdf(std::string results_file);
    void saveFitHistograms(std::string filename,
            std::map<std::string, RooDataSet*> dataMap);
    RooCategory * getCategory();

private:
    RooSimultaneous * makePdf(VarMap & vars, PdfMap & pdfs, bool toy_gen);
    std::map<std::string, double> * readFitResult(std::string results_file);

    VarMap m_fit_vars;
    VarMap m_gen_vars;
    PdfMap m_fit_pdfs;
    PdfMap m_gen_pdfs;
    std::vector<std::string> m_modes;
    RooCategory * m_cat;
    bool m_sum;
    RooRealVar * m_x;
    ParameterReader * m_pr;

};
