#include <map> 
#include <string> 
#include <vector> 

class TwoAndFourBodyPdfMaker;

// ===================================================
// Class for fitting with a range of cuts on variables
// ===================================================
class CutFitter {

public:
    CutFitter();

    void AddCuts(std::string mode, std::string varname, double min, double max,
            double increment);
    void AddBDTcuts(std::string mode, double min, double max, double increment);
    void PeformStudy(std::string filename);


private:
    std::map<std::string, std::map<std::string, std::vector<double>>> m_cuts;
    TwoAndFourBodyPdfMaker * m_pdf;

};
