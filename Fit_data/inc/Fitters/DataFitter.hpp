#ifndef DATA_FITTER_HPP
#define DATA_FITTER_HPP

#include <map>
#include <vector>
#include <string>
#include "RooDataSet.h"
#include "RooDataHist.h"

class RooDataHist;
class RooAbsData;
class RooDataSet;
class RooRealVar;
class RooArgList;
class RooFitResult;
class ShapeMakerBase;

// =========================
// Class for fitting to data
// =========================
class DataFitter {

public:
    DataFitter(ShapeMakerBase * pdf_maker, bool split = false);
    ~DataFitter();
    
    // Add files/arguments to data
    void AddFile(std::string mode, std::string filepath);
    void AddArg(std::string mode, std::string arg_name, double min, double max);

    // Fit to data
    void PerformFit(std::string filename);
    virtual RooDataHist * GetData();
    virtual RooDataSet * GetUnbinnedData();
    std::map<std::string, RooDataSet*> GetUnbinnedDataMap();

protected:
    RooFitResult * PerformFit(std::string filename, RooAbsData * data);
    std::vector<std::string> m_modes;
    ShapeMakerBase * m_pdf;
    void ResetArgs();
    void PrintArgs();

private:
    std::map<std::string, std::vector<std::string>> m_files;
    std::map<std::string, RooRealVar*> m_vars;
    std::map<std::string, RooArgList*> m_args;
    RooRealVar * m_ID;
    bool m_split;
};

#endif // DATA_FITTER_HPP
