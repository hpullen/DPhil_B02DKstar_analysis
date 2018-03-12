#ifndef TWO_BODY_FITTER_HPP
#define TWO_BODY_FITTER_HPP

#include "DataFitter.hpp"

namespace Data {

    enum Mode {
        Kpi,
        piK,
        KK,
        pipi,
        Kpipipi,
        piKpipi,
        pipipipi
    };

    inline std::string GetModeString(Mode mode) {
        switch (mode) {
            case (Mode::Kpi) : return "Kpi";
            case (Mode::piK) : return "piK";
            case (Mode::KK) : return "KK";
            case (Mode::pipi) : return "pipi";
            case (Mode::Kpipipi) : return "Kpipipi";
            case (Mode::piKpipi) : return "piKpipi";
            case (Mode::pipipipi) : return "pipipipi";
        }
        return "";
    }
}

class RooCategory;

// ===========================================
// Class for fitting to a set of two body data
// ===========================================
class TwoAndFourBodyFitter : public DataFitter {

public:
    TwoAndFourBodyFitter(bool split);
    ~TwoAndFourBodyFitter();

    void AddFile(Data::Mode mode, std::string filepath);
    void AddArg(Data::Mode mode, std::string arg_name, double min, double max);

    void PerformFit(std::string results_file, std::string hist_file);

private:
    RooRealVar * MakeFitVariable();
    RooCategory * MakeCategory(bool split);
};

#endif // TWO_BODY_FITTER
