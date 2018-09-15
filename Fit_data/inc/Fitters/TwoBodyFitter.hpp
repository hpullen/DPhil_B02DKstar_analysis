#ifndef TWO_BODY_FITTER_HPP
#define TWO_BODY_FITTER_HPP

#include "DataFitter.hpp"

namespace TwoBody {

    enum Mode {
        Kpi,
        piK,
        KK,
        pipi
    };

    inline std::string GetModeString(Mode mode) {
        switch (mode) {
            case (Mode::Kpi) : return "Kpi";
            case (Mode::piK) : return "piK";
            case (Mode::KK) : return "KK";
            case (Mode::pipi) : return "pipi";
        }
        return "";
    }
}

class RooCategory;

// ===========================================
// Class for fitting to a set of two body data
// ===========================================
class TwoBodyFitter : private DataFitter {

public:
    TwoBodyFitter();
    ~TwoBodyFitter();

    void AddFile(TwoBody::Mode mode, std::string filepath);
    void AddArg(TwoBody::Mode mode, std::string arg_name, double min, double max);

    void PerformFit(std::string results_file, std::string hist_file);

private:
    RooRealVar * MakeFitVariable();
    RooCategory * MakeCategory();

};

#endif // TWO_BODY_FITTER
