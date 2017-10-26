#ifndef COMPARE_ROC_H
#define COMPARE_ROC_H

#include <string>
#include <map>

class TH1D;

// =====================================================
// Class for plotting several ROC curves to compare them
// =====================================================

class CompareRoc {
    
public:
    CompareRoc();
    ~CompareRoc();

    void addCurve(std::string mva_name, std::string mva_type);
    void plotCurves(std::string plotname);

private:
    std::map<std::string, TH1D *> m_curves;

};

#endif // COMPARE_ROC_H
