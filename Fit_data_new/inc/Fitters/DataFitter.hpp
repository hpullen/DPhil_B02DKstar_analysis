#ifndef DATA_FITTER_HPP
#define DATA_FITTER_HPP

#include <map>
#include <vector>
#include <string>

class RooDataHist;
class RooRealVar;
class RooArgList;
class ShapeMakerBase;

// =========================
// Class for fitting to data
// =========================
class DataFitter {

public:
    DataFitter(ShapeMakerBase * pdf_maker);
    virtual ~DataFitter() {};
    
    // Add files/arguments to data
    void AddFile(std::string mode, std::string filepath);
    void AddArg(std::string mode, std::string arg_name, double min, double max);

    // Fit to data
    void PerformFit(std::string filename);

private:
    virtual RooDataHist * GetData();

    ShapeMakerBase * m_pdf;
    std::map<std::string, std::vector<std::string>> m_files;
    std::map<std::string, RooRealVar*> m_vars;
    std::map<std::string, RooArgList*> m_args;
};

#endif // DATA_FITTER_HPP
