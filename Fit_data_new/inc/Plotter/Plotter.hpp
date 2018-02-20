#ifndef PLOTTER_HPP
#define PLOTTER_HPP

#include <string>
#include <map>

enum DrawStyle {
    Points,
    Line,
    Filled
};


class TFile;
class TH1;
class THStack;
class TLegend;
class RooDataHist;

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

    void AddPulls(std::string name_in_file);
    void AddPulls(std::string mode, std::string name_in_file);

    void Draw();

private:
    TFile * m_histfile;
    std::string m_outname;
    std::vector<std::string> m_modes;
    std::map<std::string, std::vector<TH1*>> m_points;
    std::map<std::string, std::vector<TH1*>> m_lines;
    std::map<std::string, THStack*> m_stacks;
    std::map<std::string, RooHist*> m_pulls;
    std::map<std::string, TLegend*> m_leg;

    TH1F * MakeHistogram(std::string mode, std::string name_in_file, 
            DrawStyle style, int colour);
    void LoadDefaults();
    bool IsInFile(std::string item);

};

#endif // PLOTTER_HPP
