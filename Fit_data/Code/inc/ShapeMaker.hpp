#include <string>

#include "RooAbsReal.h"
#include "RooSimultaneous.h"

// Forward declarations
class ParameterReader;
class RooAbsPdf;
class RooCategory;
class RooRealVar;
class RooDataSet;

// Map of strings and RooAbsReal
typedef std::map<std::string, RooAbsReal*> VarMap;
typedef std::map<std::string, RooAbsPdf*> PdfMap;
typedef std::map<std::string, int> YieldMap;

// ========================================
// Class for creating PDFs for toys or data
// ========================================
class ShapeMaker {

public:
    ShapeMaker(std::string sum, RooRealVar * Bd_M);
    ShapeMaker(std::string sum, RooRealVar * Bd_M, bool fourBody);
    ~ShapeMaker();

    RooSimultaneous * makeFitPdf(bool blind = true);
    RooSimultaneous * makeFitPdf(const YieldMap & max_yields, bool blind = true);
    RooSimultaneous * makeZeroYieldPdf();
    RooSimultaneous * makeGenerationPdf(std::string results_file);
    RooSimultaneous * makePreviousAnalysisPdf();
    RooSimultaneous * make_2017_pdf();
    void saveFitHistograms(std::string filename,
            std::map<std::string, RooDataSet*> dataMap, bool blind = true);
    RooCategory * getCategory();
    YieldMap getExpectedYields();
    RooAbsReal * getFitVariable(std::string name);
    void shiftVariable(std::string name, double sigma);

private:
    void setup();
    RooSimultaneous * makeFitPdf(const YieldMap & max_yields, bool blind, 
            bool zero_piK_yield);
    RooSimultaneous * makePdf(VarMap & vars, PdfMap & pdfs, bool toy_gen);
    std::map<std::string, double> * readFitResult(std::string results_file);
    void makeTwoBodyPdfs(VarMap & vars, PdfMap & pdfs, bool toy_gen);
    RooSimultaneous * makeSimPdf(PdfMap & pdfs, bool toy_gen);
    void makeFloatingFourBodyVars(const YieldMap & max_yields, bool blind);
    void makeFourBodyPdfs(VarMap & vars, PdfMap & pdfs, bool toy_gen);
    void makeGenFourBodyVars(std::map<std::string, double> * results);
    void setTwoBodyConstants(VarMap & vars, bool toy_gen);

    VarMap m_fit_vars;
    VarMap m_gen_vars;
    PdfMap m_fit_pdfs;
    PdfMap m_gen_pdfs;
    std::vector<std::string> m_modes;
    std::vector<std::string> m_4_modes;
    RooCategory * m_cat;
    bool m_sum;
    RooRealVar * m_x;
    ParameterReader * m_pr;
    bool m_yieldsCalculated;
    bool m_fourBody;
    YieldMap m_expectedYields;

};
