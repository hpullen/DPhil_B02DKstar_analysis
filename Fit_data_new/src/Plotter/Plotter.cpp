#include "TFile.h"
#include "TH1F.h"

#include "Plotter.hpp"


// ===========
// Constructor
// ===========
Plotter::Plotter(std::string hist_file, std::string outname, 
        std::vector<std::string> modes) :
    m_histfile(TFile::Open(hist_file.c_str(), "READ")),
    m_outname(outname),
    m_modes(modes),
    m_leg(new TLegend(0.55, 0.45, 0.85, 0.9)) {

    // Set up maps
    for (auto mode : modes) {
        m_points.emplace(mode, {});
        m_lines.emplace(mode, {});
        m_stacks.emplace(mode, new THStack(("stack_" + mode).c_str(), ""));
    }
}


// ==========
// Destructor
// ==========
Plotter::~Plotter() {
    m_histfile->Close();
}




