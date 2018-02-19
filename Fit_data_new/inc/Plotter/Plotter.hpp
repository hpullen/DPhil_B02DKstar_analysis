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


// =========================================
// Class for plotting histograms from a file
// =========================================
class Plotter {

public:
    Plotter(std::string hist_file, std::string outname, 
            std::vector<std::string> modes);
    ~Plotter();

    AddComponent(std::string name_in_file, DrawStyle style, int colour);
    AddComponent(std::string name_in_file, DrawStyle style, int colour, 
            std::string legend);
    AddComponent(std::string mode, std::string name_in_file, DrawStyle style, int colour);
    AddComponent(std::string mode, std::string name_in_file, DrawStyle style, int colour, 
            std::string legend);

    Draw();

private:
    TFile * m_histfile;
    std::string m_outname;
    std::vector<std::string> m_modes;
    std::map<std::string, std::vector<TH1*>> m_points;
    std::map<std::string, std::vector<TH1*>> m_lines;
    std::map<std::string, TStack*> m_stacks;
    TLegend * m_leg;

};

#endif // PLOTTER_HPP
