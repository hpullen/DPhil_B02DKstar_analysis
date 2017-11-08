#ifndef PLOTTER_H
#define PLOTTER_H

#include <string>

class TCanvas;

// ==============================
// Class for plotting fit results
// ==============================
class Plotter {

public:
    Plotter();
    ~Plotter();

    void plotTwoModeFits(std::string histofile, std::string plotname, 
            std::string saveDir);
    void plotTwoModeFitsCombined(std::string histofile, std::string plotname, 
            std::string saveDir);
    void plotFourModeFitsCombined(std::string histofile, std::string plotname, 
            std::string saveDir);
    void plotFourModeFitsSeparate(std::string histofile, std::string plotname, 
            std::string saveDir);
    void plotFourBodyFitsCombined(std::string histofile, std::string plotname,
            std::string saveDir);
    void plotFourBodyFitsSeparate(std::string histofile, std::string plotname,
            std::string saveDir);

private:
    std::string m_current_name;
    std::string m_current_histfile;

    void plotKpiFit(std::string flav, TCanvas * all_canvas, int plot_number,
            std::string saveDir);
    void plotBlindFit(std::string mode, std::string flav, TCanvas * all_canvas, 
            int plot_number, std::string saveDir);

};

#endif // PLOTTER_H
