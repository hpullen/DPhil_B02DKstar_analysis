#include <map> 
#include <string> 
#include <vector> 

class TTree;
class RooDataHist;
class DataPdfMaker;
class ShapeMakerBase;


// ===================================================
// Class for fitting with a range of cuts on variables
// ===================================================
class CutFitter {

public:
    CutFitter(ShapeMakerBase * pdf);
    ~CutFitter();

    // Add data files
    void AddFile(std::string mode, std::string filepath);

    // Set BDT cut ranges and perform a study
    void SetCut(std::string mode, double cut);
    void SetCut(std::string, double min, double max, double increment);
    void PerformStudy(std::string filename);


private:
    // List of cuts and input files for each mode
    std::map<std::string, std::vector<double>> m_cuts;
    std::map<std::string, std::vector<std::string>> m_files;

    // Fit PDF
    ShapeMakerBase * m_pdf;

    // Setup functions
    std::map<std::string, std::vector<double>> MakeDefaultCuts();
    std::vector<std::map<std::string, double>> MakeCutsList();
    std::map<std::string, double> GetBranches();

    // Data getting functions
    std::map<std::string, RooDataHist*> GetFullDataset();
    RooDataHist * GetCutDataset(std::map<std::string, RooDataHist*> full_data,
            std::map<std::string, double> cuts);

};
