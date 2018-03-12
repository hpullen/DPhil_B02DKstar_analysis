#include <map> 
#include <string> 
#include <vector> 

#include "TwoAndFourBodyFitter.hpp"

class TTree;
class DataPdfMaker;

// ===================================================
// Class for fitting with a range of cuts on variables
// ===================================================
class CutFitter : public TwoAndFourBodyFitter {

public:
    CutFitter();
    ~CutFitter();

    // Set BDT cut ranges and perform a study
    void SetCut(Data::Mode mode, double cut);
    void SetCut(Data::Mode, double min, double max, double increment);
    void PerformStudy(std::string filename);


private:
    std::map<Data::Mode, std::vector<double>> m_cuts;
    std::map<Data::Mode, std::vector<double>> MakeDefaultCuts();
    std::vector<std::map<Data::Mode, double>> MakeCutsList();
    std::map<std::string, double> GetBranches();

};
