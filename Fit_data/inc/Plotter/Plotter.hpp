#ifndef PLOTTER_HPP
#define PLOTTER_HPP

#include <string>
#include <map>

#include "TColor.h"

#include "PlotStyle.hpp"

enum DrawStyle {
    Points,
    Line,
    Filled
};

// Custom colours
// TColor * col = new TColor();
// int hTurquoise = col->GetColor(102, 194, 165);
// int hRed = col->GetColor(252, 141, 98);
// int hBlue = col->GetColor(141, 160, 203);
// int hPink = col->GetColor(231, 138, 195);
// int hGreen = col->GetColor(166, 216, 84);
// int hYellow = col->GetColor(255, 217, 47);
// int hBeige = col->GetColor(229, 196, 148);

class TFile;
class TH1F;
class THStack;
class TLegend;
class RooDataHist;
class RooPlot;

// =========================================
// Class for plotting histograms from a file
// =========================================
class Plotter {

public:
    Plotter(std::string hist_file, std::string outname, 
            std::vector<std::string> modes,
            bool sum_runs = false);
    ~Plotter();

    void AddComponent(std::string name_in_file, DrawStyle style, int colour);
    void AddComponent(std::string name_in_file, DrawStyle style, int colour, 
            std::string legend);
    void AddComponent(std::string mode, std::string name_in_file, DrawStyle style, 
            int colour);
    void AddComponent(std::string mode, std::string name_in_file, DrawStyle style,
            int colour, std::string legend);

    void AddPulls(std::string mode, std::string name_in_file);

    void MatchScales();
    bool IsInFile(std::string item);

    virtual void Draw();

protected:
    TFile * m_histfile;
    std::pair<TH1F*, DrawStyle> GetTallest(std::string mode);
    std::string m_outname;
    std::vector<std::string> m_modes;
    std::map<std::string, std::vector<TH1F*>> m_points;
    std::map<std::string, std::vector<TH1F*>> m_lines;
    std::map<std::string, THStack*> m_stacks;
    std::map<std::string, RooPlot*> m_pulls;
    std::map<std::string, TLegend*> m_leg;


    void SetTitles(TH1F * hist, std::string mode);
    std::string ConvertToLatex(std::string mode);
    std::string AutoLegend(std::string mode, std::string name);

    bool m_sum_runs;
    bool m_rescale;
    std::map<std::string, double> m_scales;

private:
    TH1F * MakeHistogram(std::string mode, std::string name_in_file, 
            DrawStyle style, int colour);
    void LoadDefaults();

};

#endif // PLOTTER_HPP

