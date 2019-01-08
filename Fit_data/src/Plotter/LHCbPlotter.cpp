#include "TROOT.h"

#include "TCanvas.h"
#include "TROOT.h"
#include "TFile.h"
#include "TH1F.h"
#include "THStack.h"
#include "TLegend.h"
#include "TLine.h"
#include "TStyle.h"
#include "TPad.h"

#include "lhcbStyle.h"
#include "LHCbPlotter.hpp"


// ===========
// Constructor
// ===========
LHCbPlotter::LHCbPlotter(std::string hist_file, std::string outname, 
        std::vector<std::string> modes) : 
    Plotter(hist_file, outname, modes) {

    // Set plotting style
    setStyle();
}


// ==========
// Destructor
// ==========
LHCbPlotter::~LHCbPlotter() {
    m_histfile->Close();
}


// ========================
// Draw and save histograms
// ========================
void LHCbPlotter::Draw() {

    // Get rescaling factors for split fit
    std::cout << "Drawing plots in LHCb paper style" << std::endl;
    setStyle();
    MatchScales();

    // Loop through modes and draw for each
    for (auto mode : m_modes) {

        // Make canvas
        TCanvas * canvas = new TCanvas(("canvas_" + mode).c_str(), "", 900, 600);

        // Plot the tallest histogram first
        if (m_lines[mode].size() > 0 && m_points[mode].size() > 0) {
            std::pair<TH1F*, DrawStyle> tallest = GetTallest(mode);
            TString opt = (tallest.second == DrawStyle::Line) ? "C" : "E";
            SetTitles(tallest.first, mode);
            if (m_rescale) {
                tallest.first->GetYaxis()->SetRangeUser(0, m_scales[mode] * 1.2);
            }
            tallest.first->Draw(opt + " SAME");
        }

        // Draw all histograms
        if (m_stacks[mode]->GetNhists() > 0) {
            m_stacks[mode]->Draw("C SAME");
        }
        for (auto line : m_lines[mode]) line->Draw("C SAME");
        for (auto line : m_lines[mode]) {
            if (line->GetLineColor() == kBlack) line->Draw("C SAME");
        }
        for (auto points : m_points[mode]) points->Draw("SAME");
        gPad->RedrawAxis();

        // Draw legend
        m_leg[mode]->SetY1(0.9 - (0.07) * m_leg[mode]->GetNRows());
        m_leg[mode]->Draw();

        // Draw label
        getNameLabel(true)->Draw();

        // Draw "blind" label if not favoured
        if (mode.find("Kpi") != 0) {
            getBlindLabel()->Draw();
        }

        // Save
        canvas->SaveAs((m_outname + "_" + mode + ".pdf").c_str());
    }
}
