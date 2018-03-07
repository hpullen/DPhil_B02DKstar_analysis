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
            std::vector<std::string> modes);
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

    void Draw();

private:
    TFile * m_histfile;
    std::string m_outname;
    std::vector<std::string> m_modes;
    std::map<std::string, std::vector<TH1F*>> m_points;
    std::map<std::string, std::vector<TH1F*>> m_lines;
    std::map<std::string, THStack*> m_stacks;
    std::map<std::string, RooPlot*> m_pulls;
    std::map<std::string, TLegend*> m_leg;

    TH1F * MakeHistogram(std::string mode, std::string name_in_file, 
            DrawStyle style, int colour);
    void LoadDefaults();
    bool IsInFile(std::string item);
    std::pair<TH1F*, DrawStyle> GetTallest(std::string mode);
    void SetTitles(TH1F * hist, std::string mode);
    std::string ConvertToLatex(std::string mode);

    bool m_rescale;
    std::map<std::string, double> m_scales;

};

#endif // PLOTTER_HPP
