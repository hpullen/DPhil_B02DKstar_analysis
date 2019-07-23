#ifndef LHCB_PLOTTER_HPP
#define LHCB_PLOTTER_HPP

#include <string>
#include <map>

#include "Plotter.hpp"

// =========================================
// Class for plotting histograms from a file
// =========================================
class LHCbPlotter : public Plotter {

public:
    LHCbPlotter(std::string hist_file, std::string outname, 
            std::vector<std::string> modes, bool sum_runs);
    ~LHCbPlotter();

    void Draw(bool zoomed = false, bool log = false);

};

#endif // LHCB_PLOTTER_HPP

