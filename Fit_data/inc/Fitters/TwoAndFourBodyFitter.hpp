#ifndef TWO_BODY_FITTER_HPP
#define TWO_BODY_FITTER_HPP

#include <iostream>

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

    enum Run {
        Run1,
        Run2,
        Both,
        BothCombined
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

    inline Mode GetModeFromString(std::string mode) {
        if (mode == "Kpi")  return Mode::Kpi;
        else if (mode == "piK")  return Mode::piK;
        else if (mode == "KK")  return Mode::KK;
        else if (mode == "pipi")  return Mode::pipi;
        else if (mode == "Kpipipi")  return Mode::Kpipipi;
        else if (mode == "piKpipi")  return Mode::piKpipi;
        else if (mode == "pipipipi")  return Mode::pipipipi;
        else {
            std::cout << "Error in GetModeFromString: mode "
                << mode << " not recognised." << std::endl;
            exit (EXIT_FAILURE);
        }
    }
}

class RooCategory;

// ===========================================
// Class for fitting to a set of two body data
// ===========================================
class TwoAndFourBodyFitter : public DataFitter {

public:
    TwoAndFourBodyFitter(bool split, Data::Run run_opt, std::vector<std::string> modes, bool split_GLW = true);
    ~TwoAndFourBodyFitter();

    void AddFile(Data::Mode mode, std::string filepath);
    void AddFile(Data::Mode mode, int run, std::string filepath);
    void AddArg(Data::Mode mode, std::string arg_name, double min, double max);

    void SeparateRruns();

    double PerformFit(std::string results_file, std::string hist_file, bool binned);
    double PerformSignificanceFit(std::string mode, std::string results_file,
            std::string hist_file, bool binned);

private:
    bool m_split_GLW;
    RooRealVar * MakeFitVariable();
    RooCategory * MakeCategory(bool split, Data::Run run_opt, std::vector<std::string> modes);
};

#endif // TWO_BODY_FITTER
