#include <iostream> 
#include <sstream> 

#include "TCanvas.h"
#include "TROOT.h"
#include "TFile.h"
#include "TH1F.h"
#include "THStack.h"
#include "TLegend.h"
#include "TLine.h"
#include "TStyle.h"
#include "TPad.h"

#include "RooHist.h"
#include "RooPlot.h"

#include "Plotter.hpp"


// ===========
// Constructor
// ===========
Plotter::Plotter(std::string hist_file, std::string outname, 
        std::vector<std::string> modes) :
    m_histfile(TFile::Open(hist_file.c_str(), "READ")),
    m_outname(outname),
    m_modes(modes) {

    // Set plotting style
    setPlotStyle();

    // Set up maps
    for (auto mode : modes) {
        m_points[mode] = {};
        m_lines[mode] = {};
        m_stacks.emplace(mode, new THStack(("stack_" + mode).c_str(), ""));
        m_leg.emplace(mode, new TLegend(0.6, 0.45, 0.85, 0.9));
    }

    // Attempt to load data, fit and pulls
    LoadDefaults();
}


// ==========
// Destructor
// ==========
Plotter::~Plotter() {
    m_histfile->Close();
}


// ==============================
// Add a component for every mode
// ==============================
void Plotter::AddComponent(std::string name_in_file, DrawStyle style, int colour) {
    for (auto mode : m_modes) {
        AddComponent(mode, name_in_file, style, colour);
    }
}

void Plotter::AddComponent(std::string name_in_file, DrawStyle style, int colour,
        std::string legend) {
    for (auto mode : m_modes) {
        AddComponent(mode, name_in_file, style, colour, legend);
    }
}


// ===============
// Add a component
// ===============
void Plotter::AddComponent(std::string mode, std::string name_in_file,
        DrawStyle style, int colour) {
    MakeHistogram(mode, name_in_file + "_" + mode, style, colour);
}


// ===================================
// Add a component with a legend entry
// ===================================
void Plotter::AddComponent(std::string mode, std::string name_in_file,
        DrawStyle style, int colour, std::string legend) {
    TH1F * hist = MakeHistogram(mode, name_in_file + "_" + mode, style, colour);
    m_leg[mode]->AddEntry(hist, legend.c_str());
}


// ==================
// Add pull histogram
// ==================
void Plotter::AddPulls(std::string mode, std::string name_in_file) {

    // Check item is in file
    if (!IsInFile(name_in_file)) {
        std::cout << "Item " << name_in_file << " not found in file! " <<
            std::endl;
        exit (EXIT_FAILURE);
    }

    // Get the pull histogram
    RooHist * pulls = (RooHist*)m_histfile->Get(name_in_file.c_str());

    // Get range for pulls plot
    TAxis * axis;
    if (m_lines[mode].size() > 0 && m_points[mode].size() > 0) {
        axis = GetTallest(mode).first->GetXaxis();
    } else {
        axis = pulls->GetXaxis();
    }
    double x_min = axis->GetXmin();
    double x_max = axis->GetXmax();

    // Set attributes and draw
    pulls->SetFillColor(kBlue + 3);
    RooPlot * plot = new RooPlot(x_min, x_max);
    plot->SetMinimum(-5);
    plot->SetMaximum(5);
    plot->addPlotable(pulls, "BEX0");

    // Add to map
    m_pulls.emplace(mode, plot);
}


// ========================
// Draw and save histograms
// ========================
void Plotter::Draw() {

    // Loop through modes and draw for each
    for (auto mode : m_modes) {

        // Make canvas
        TCanvas * canvas;
        TPad * pad1;
        if (m_pulls.find(mode) != m_pulls.end()) {
            canvas = new TCanvas(("canvas_" + mode).c_str(), "", 1.25 * 700, 1000);
            pad1 = new TPad(("plots_" + mode).c_str(), "", 0.0, 0.3, 1.0, 1.0);
        } else {
            canvas = new TCanvas(("canvas_" + mode).c_str(), "", 500, 400);
            pad1 = new TPad(("plots_" + mode).c_str(), "", 0.0, 0.0, 1.0, 1.0);
        }
        pad1->cd();

        // Plot the tallest histogram first
        if (m_lines[mode].size() > 0 && m_points[mode].size() > 0) {
            std::pair<TH1F*, DrawStyle> tallest = GetTallest(mode);
            TString opt = (tallest.second == DrawStyle::Line) ? "C" : "E";
            SetTitles(tallest.first, mode);
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
        m_leg[mode]->SetY1(0.9 - (0.06) * m_leg[mode]->GetNRows());
        m_leg[mode]->Draw();

        // Draw on canvas
        canvas->cd();
        pad1->Draw();

        // Draw pulls
        if (m_pulls.find(mode) != m_pulls.end()) {

            // Make pull plot
            TPad * pad2 = new TPad(("pullpad_" + mode).c_str(), "", 0, 0, 1, 0.3);
            pad2->cd();
            m_pulls[mode]->Draw();

            // Add lines at +/- 3
            TLine * line = new TLine(5000, -3, 5800, -3);
            line->SetLineStyle(2);
            line->SetLineColor(kRed + 2);
            line->Draw();
            TLine * line2 = new TLine(5000, 3, 5800, 3);
            line2->SetLineStyle(2);
            line2->SetLineColor(kRed + 2);
            line2->Draw();

            // Draw on main canvas
            canvas->cd();
            pad2->Draw();
        }

        // Save
        canvas->SaveAs((m_outname + "_" + mode + ".pdf").c_str());
    }
}


// =============================
// Make histogram and add to map
// =============================
TH1F * Plotter::MakeHistogram(std::string mode, std::string name_in_file,
        DrawStyle style, int colour) {

    // Check for item
    if (!IsInFile(name_in_file)) {
        std::cout << "Item " << name_in_file << " not found in file! " <<
            std::endl;
        exit (EXIT_FAILURE);
    }

    // Get histogram from file
    gROOT->ForceStyle();
    TH1F * hist = (TH1F*)m_histfile->Get(name_in_file.c_str());

    // Set attributes
    hist->SetLineColor(colour);
    hist->SetMarkerColor(colour);
    hist->SetStats(kFALSE);
    if (style == DrawStyle::Points) {
        hist->SetLineWidth(1);
        hist->SetMarkerStyle(8);
        hist->SetMarkerSize(1);
    } else if (style == DrawStyle::Line) {
        hist->SetLineWidth(2);
        hist->SetMarkerStyle(0);
        hist->SetMarkerSize(0);
    } else if (style == DrawStyle::Filled) {
        hist->SetFillColor(colour);
        hist->SetLineWidth(0);
        hist->SetLineColor(0);
        hist->SetMarkerStyle(0);
        hist->SetMarkerSize(0);
    }

    // Add to relevant map
    switch (style) {
        case (DrawStyle::Points) : m_points[mode].push_back(hist); 
                                   break;
        case (DrawStyle::Line) : m_lines[mode].push_back(hist); 
                                 break;
        case(DrawStyle::Filled) : m_stacks[mode]->Add(hist);
                                  break;
    }

    // Return histogram
    return hist;
}


// =======================
// Load default histograms
// =======================
void Plotter::LoadDefaults() {

    // Loop through modes 
    std::stringstream data_stream;
    std::stringstream fit_stream;
    std::stringstream pull_stream;
    for (auto mode : m_modes) {

        // Datapoints
        if (IsInFile("data_" + mode)) {
            AddComponent(mode, "data", DrawStyle::Points, kBlack, "Data");
        }

        // Overall fit
        if (IsInFile("fit_" + mode)) {
            AddComponent(mode, "fit", DrawStyle::Line, kBlack, "Fit");
        }

        // Pulls
        if (IsInFile("pulls_" + mode)) {
            AddPulls(mode, "pulls_" + mode);
        }
    }
}


// ========================================
// Check if histogram file contains an item
// ========================================
bool Plotter::IsInFile(std::string item) {
    return (m_histfile->GetListOfKeys()->Contains(item.c_str()));
}


// ==============================================
// Return pointer to tallest histogram for a mode
// ==============================================
std::pair<TH1F*, DrawStyle> Plotter::GetTallest(std::string mode) {
    double max = 0;
    TH1F * tallest;
    DrawStyle style;
    for (auto line : m_lines[mode]) {
        if (line->GetMaximum() > max) {
            tallest = line;
            max = line->GetMaximum();
            style = DrawStyle::Line;
        }
    }
    for (auto points : m_points[mode]) {
        if (points->GetMaximum() > max) {
            tallest = points;
            max = points->GetMaximum();
            style = DrawStyle::Points;
        }
    }
    return std::make_pair(tallest, style);
}


// ===================================
// Apply X and Y titles to a histogram
// ===================================
void Plotter::SetTitles(TH1F * hist, std::string mode) {

    // X label based on mode
    std::string x_label = "#it{m}([" + ConvertToLatex(mode) + "]_{D}K^{*0}) "
        "[MeV/#it{c}^{2}]";
    hist->GetXaxis()->SetTitle(x_label.c_str());

    // Y label based on binning
    std::stringstream width;
    std::string hist_name = hist->GetName();
    width << std::setprecision(1) << hist->GetXaxis()->GetBinWidth(1) * 10;
    std::string y_label = "Candidates / (" + width.str() + " MeV/#it{c}^{2})";
    hist->GetYaxis()->SetTitle(y_label.c_str());

}


// =============================================
// Convert a mode string to a Latex-style string
// =============================================
std::string Plotter::ConvertToLatex(std::string mode) {
    std::string mode_short = mode.substr(0, mode.find("_plus"));
    mode_short = mode_short.substr(0, mode_short.find("_minus"));
    if (mode_short == "Kpi") return "K#pi";
    if (mode_short == "piK") return "#piK";
    if (mode_short == "pipi") return "#pi#pi";
    if (mode_short == "Kpipipi") return "K#pi#pi#pi";
    if (mode_short == "piKpipi") return "#piK#pi#pi";
    if (mode_short == "pipipipi") return "#pi#pi#pi#pi";
    else return mode;
}
