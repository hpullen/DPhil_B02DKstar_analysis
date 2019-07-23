#include <iostream> 
#include <sstream> 
#include <iomanip>

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
        std::vector<std::string> modes, bool sum_runs) :
    m_histfile(TFile::Open(hist_file.c_str(), "READ")),
    m_outname(outname),
    m_modes(modes),
    m_sum_runs(sum_runs),
    m_rescale(false) {

    // Set plotting style
    setPlotStyle();

    // Set up maps
    for (auto mode : modes) {
        m_points[mode] = {};
        m_lines[mode] = {};
        m_stacks.emplace(mode, new THStack(("stack_" + mode).c_str(), ""));
        m_leg.emplace(mode, new TLegend(0.6, 0.45, 0.9, 0.92));
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

    // See if autolegend can be made
    TH1F * hist = MakeHistogram(mode, name_in_file + "_" + mode, style, colour);
    std::string leg = AutoLegend(mode, name_in_file);
    if (leg != "") {
        m_leg[mode]->AddEntry(hist, leg.c_str());
    } 
}


// ===================================
// Add a component with a legend entry
// ===================================
void Plotter::AddComponent(std::string mode, std::string name_in_file,
        DrawStyle style, int colour, std::string legend) {
    std::cout << "Looking for " << name_in_file << " for " << mode << std::endl;
    TH1F * hist = MakeHistogram(mode, name_in_file + "_" + mode, style, colour);
    m_leg[mode]->AddEntry(hist, legend.c_str());
}


// ==================
// Add pull histogram
// ==================
void Plotter::AddPulls(std::string mode, std::string name_in_file) {

    // If summing runs, check for run 1 version
    std::string name_run1 = "";
    std::string name_run2 = "";
    bool is_4pi = mode.find("pipipipi") != std::string::npos;
    bool sum_this_mode = m_sum_runs && !is_4pi;
    if (sum_this_mode) {
        for (std::string flav : {"plus", "minus"}) {
            size_t pos = name_in_file.find(flav);
            if (pos != std::string::npos) {
                name_run1 = name_in_file.substr(0, pos) + "run1_" 
                    + name_in_file.substr(pos, std::string::npos);
                name_run2 = name_in_file.substr(0, pos) + "run2_" 
                    + name_in_file.substr(pos, std::string::npos);
                break;
            } else {
                name_run1 = name_in_file + "_run1";
                name_run2 = name_in_file + "_run2";
            }
        }
        if (name_in_file.find("Bs_low") != std::string::npos) {
            size_t pos = name_run1.find("Bs_low_") + strlen("Bs_low_");
            name_run1 = name_run1.substr(0, pos) + "run1_" +
                name_run1.substr(pos, std::string::npos);
            name_run2 = name_run2.substr(0, pos) + "run2_" +
                name_run2.substr(pos, std::string::npos);
        }
    }

    // Check for item
    if (!sum_this_mode) {
        if (!IsInFile(name_in_file)) {
            std::cout << "Item " << name_in_file << " not found in file! " <<
                std::endl;
            exit (EXIT_FAILURE);
        }
    } else {
        if (!IsInFile(name_run1)) {
            std::cout << "Item " << name_run1 << " not found in file! " <<
                std::endl;
            exit (EXIT_FAILURE);
        }
        if (!IsInFile(name_run2)) {
            std::cout << "Item " << name_run2 << " not found in file! " <<
                std::endl;
            exit (EXIT_FAILURE);
        }
    }

    // Get the pull histogram
    RooHist * pulls; 
    RooHist * pulls_run1;
    RooHist * pulls_run2;
    if (!sum_this_mode) {
        pulls = (RooHist*)m_histfile->Get(name_in_file.c_str());
    } else { 
        pulls_run1 = (RooHist*)m_histfile->Get(name_run1.c_str());
        pulls_run2 = (RooHist*)m_histfile->Get(name_run2.c_str());
        pulls= new RooHist(*pulls_run1, *pulls_run2);
    }

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


// ======================================================================
// Get axis heights for each mode so that plus and minus histograms match
// ======================================================================
void Plotter::MatchScales() {

    // Loop through modes
    for (auto mode : m_modes) {

        // Skip if already done
        if (m_scales.find(mode) != m_scales.end()) continue;

        // Find out whether mode is plus or minus
        std::string equiv_mode;
        if (mode.find("_plus") != std::string::npos) {
            m_rescale = true;
            equiv_mode = mode.substr(0, mode.find("_plus")) + "_minus";
        } else if (mode.find("_minus") != std::string::npos) {
            m_rescale = true;
            equiv_mode = mode.substr(0, mode.find("_minus")) + "_plus";
        } else return;

        // Get tallest histogram of each mode
        double max_1 = GetTallest(mode).first->GetMaximum();
        double max_2 = GetTallest(equiv_mode).first->GetMaximum();
        double max_height = (max_1 > max_2) ? max_1 : max_2;
        m_scales[mode] = max_height;
        m_scales[equiv_mode] = max_height;
    }
}


// ========================
// Draw and save histograms
// ========================
void Plotter::Draw(bool zoomed, bool log) {

    // Get rescaling factors for split fit
    MatchScales();

    // Loop through modes and draw for each
    for (auto mode : m_modes) {

        // Make canvas
        TCanvas * canvas;
        TPad * pad1;
        if (m_pulls.find(mode) != m_pulls.end()) {
            canvas = new TCanvas(("canvas_" + mode).c_str(), "", 900, 900);
            pad1 = new TPad(("plots_" + mode).c_str(), "", 0.0, 1.0/3.0, 1.0, 1.0);
        } else {
            canvas = new TCanvas(("canvas_" + mode).c_str(), "", 900, 600);
            pad1 = new TPad(("plots_" + mode).c_str(), "", 0.0, 0.0, 1.0, 1.0);
        }
        pad1->cd();

        // Plot the tallest histogram first
        if (m_lines[mode].size() > 0 && m_points[mode].size() > 0) {
            std::pair<TH1F*, DrawStyle> tallest = GetTallest(mode);
            TString opt = (tallest.second == DrawStyle::Line) ? "C" : "E";
            SetTitles(tallest.first, mode);
            if (m_rescale) {
                tallest.first->GetYaxis()->SetRangeUser(0.00001, m_scales[mode] * 1.2);
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
        m_leg[mode]->SetFillStyle(0);
        m_leg[mode]->SetFillColor(0);
        m_leg[mode]->Draw();

        // Draw on canvas
        canvas->cd();
        pad1->Draw();

        // Draw pulls
        if (m_pulls.find(mode) != m_pulls.end()) {

            // Make pull plot
            TPad * pad2 = new TPad(("pullpad_" + mode).c_str(), "", 0, 0, 1, 1.0/3.0);
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

    // If summing runs, check for run 1 version
    std::string name_run1 = "";
    std::string name_run2 = "";
    bool is_4pi = mode.find("pipipipi") != std::string::npos;
    bool sum_this_mode = m_sum_runs && !is_4pi;
    if (sum_this_mode) {
        for (std::string flav : {"plus", "minus"}) {
            size_t pos = name_in_file.find(flav);
            if (pos != std::string::npos) {
                name_run1 = name_in_file.substr(0, pos) + "run1_" 
                    + name_in_file.substr(pos, std::string::npos);
                name_run2 = name_in_file.substr(0, pos) + "run2_" 
                    + name_in_file.substr(pos, std::string::npos);
                break;
            } else {
                name_run1 = name_in_file + "_run1";
                name_run2 = name_in_file + "_run2";
            }
        }
        if (name_in_file.find("Bs_low") != std::string::npos) {
            size_t pos = name_run1.find("Bs_low_") + strlen("Bs_low_");
            name_run1 = name_run1.substr(0, pos) + "run1_" +
                name_run1.substr(pos, std::string::npos);
            name_run2 = name_run2.substr(0, pos) + "run2_" +
                name_run2.substr(pos, std::string::npos);
        }
    }

    // Check for item
    if (!sum_this_mode) {
        if (!IsInFile(name_in_file)) {
            std::cout << "Item " << name_in_file << " not found in file! " <<
                std::endl;
            exit (EXIT_FAILURE);
        }
    } else {
        if (!IsInFile(name_run1)) {
            std::cout << "Item " << name_run1 << " not found in file! " <<
                std::endl;
            exit (EXIT_FAILURE);
        }
        if (!IsInFile(name_run2)) {
            std::cout << "Item " << name_run2 << " not found in file! " <<
                std::endl;
            exit (EXIT_FAILURE);
        }
    }

    // Get histogram from file
    gROOT->ForceStyle();
    TH1F * hist;
    TH1F * hist_run2;
    if (!m_sum_runs || is_4pi) {
        hist = (TH1F*)m_histfile->Get(name_in_file.c_str());
    } else {
        hist = (TH1F*)m_histfile->Get(name_run1.c_str());
        hist_run2 = (TH1F*)m_histfile->Get(name_run2.c_str());
        hist->Add(hist_run2);
    }

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
        std::cout << "Mode: " << mode << std::endl;

        if (m_sum_runs && mode.find("pipipipi") == std::string::npos) {
            AddComponent(mode, "data", DrawStyle::Points, kBlack, "Data");
            AddComponent(mode, "fit", DrawStyle::Line, kBlack, "Fit");
            AddPulls(mode, "pulls_" + mode);
        } else {

            // Non-summing version
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
    // for (auto points : m_points[mode]) {
        // if (points->GetMaximum() > max) {
            // tallest = points;
            // max = points->GetMaximum();
            // style = DrawStyle::Points;
        // }
    // }
    return std::make_pair(tallest, style);
}


// ===================================
// Apply X and Y titles to a histogram
// ===================================
void Plotter::SetTitles(TH1F * hist, std::string mode) {

    // X label based on mode
    std::string x_label = "#it{m}([" + ConvertToLatex(mode) + "]_{#it{D}}";
    if (mode.find("minus") != std::string::npos) {
        x_label += "#it{#bar{K}}";
    } else {
        x_label += "#it{K}";
    }
    x_label += "^{*0}) [MeV/#it{c}^{2}]";
    hist->GetXaxis()->SetTitle(x_label.c_str());
    hist->GetXaxis()->SetTitleSize(0.08);
    hist->GetXaxis()->SetTitleOffset(1.1);

    // Y label based on binning
    std::stringstream width;
    std::string hist_name = hist->GetName();
    if (hist_name.find("data") != std::string::npos) {
        width << (int)(hist->GetXaxis()->GetBinWidth(1));
    } else {
        width << (int)(hist->GetXaxis()->GetBinWidth(1) * 10);
    }
    std::string y_label = "Candidates / (" + width.str() + " MeV/#it{c}^{2})";
    hist->GetYaxis()->SetTitle(y_label.c_str());
    hist->GetYaxis()->SetTitleSize(0.08);
    hist->GetYaxis()->SetTitleOffset(0.8);

    // Number size
    hist->GetXaxis()->SetLabelSize(0.08);
    hist->GetYaxis()->SetLabelSize(0.08);

}


// =============================================
// Convert a mode string to a Latex-style string
// =============================================
std::string Plotter::ConvertToLatex(std::string mode) {
    std::string mode_short = mode.substr(0, mode.find("_run1"));
    mode_short = mode_short.substr(0, mode_short.find("_run2"));
    mode_short = mode_short.substr(0, mode_short.find("_plus"));
    mode_short = mode_short.substr(0, mode_short.find("_minus"));
    if (mode_short == "Kpi") return "#it{K}#pi";
    if (mode_short == "piK") return "#pi#it{K}";
    if (mode_short == "pipi") return "#pi#pi";
    if (mode_short == "KK") return "#it{KK}";
    if (mode_short == "Kpipipi") return "#it{K}#pi#pi#pi";
    if (mode_short == "piKpipi") return "#pi#it{K}#pi#pi";
    if (mode_short == "pipipipi") return "#pi#pi#pi#pi";
    else return mode;
}


// ===================
// Make a legend entry
// ===================
std::string Plotter::AutoLegend(std::string mode, std::string name) {

    // Find background type
    std::string type;
    if (name.find("signal") != std::string::npos) {
        type = "signal";
    } else if (name.find("Bs") != std::string::npos) {
        if (name.find("low") != std::string::npos) {
            type = "Bs_low";
        } else {
            type = "Bs";
        }
    } else if (name.find("low") != std::string::npos) {
        type = "low";
    } else if (name.find("rho") != std::string::npos) {
        type = "rho";
    } else if (name.find("DKpipi") != std::string::npos) {
        type = "DKpipi";
    } else {
        return "";
    }

    // Find sign (plus or minus or neither)
    std::string sign;
    if (mode.find("plus") != std::string::npos) {
        sign = "plus";
    } else if (mode.find("minus") != std::string::npos) {
        sign = "minus";
    } else {
        sign = "";
    }

    // Start string
    std::string leg = "";
    if (type == "rho") {
        leg += "#it{B}^{0}";
    } else if (type == "signal" || type == "low") {
        if (sign == "plus" || sign == "") {
            leg += "#it{B}^{0}";
        } else {
            leg += "#it{#bar{B}}^{0}";
        }
    } else if (type == "DKpipi") {
        if (sign == "plus" || sign == "") {
            leg += "#it{B}^{+}";
        } else {
            leg += "#it{B}^{#minus}";
        }
    } else if (type == "Bs" || type == "Bs_low") {
        if (sign == "minus" || sign == "") {
            leg += "#it{B}^{0}_{s}";
        } else {
            leg += "#it{#bar{B}}^{0}_{s}";
        }
    }

    // Add arrow and D
    if (type == "low" || type == "Bs_low") {
        if (sign == "plus" || sign == "") {
            // leg += "#rightarrow#it{D}^{#lower[0.2]{*}}";
            leg += "#rightarrow#it{D}#lower[0.35]{^{*}}";
        } else {
            leg += "#rightarrow#it{D}#lower[0.45]{^{*}}";
        }
    } else {
        leg += "#rightarrow#it{D}";
    }

    // Add K* decay product
    if (type == "Bs" || type == "signal" || type == "low" || type == "Bs_low") {
        if (sign == "plus" || sign == "") {
            leg += "#it{K}^{*0}";
        } else {
            leg += "#kern[0.05]{#bar{#it{K}}#lower[0.13]{^{*0}}}";
        }
    } else if (type == "rho") {
        leg += "#kern[0.05]{#pi^{+}#pi^{#minus}}";
    } else if (type == "DKpipi") {
        if (sign == "plus" || sign == "") {
            leg += "#it{K}^{+}#pi^{#minus}#pi^{+}";
        } else {
            leg += "#it{K}^{#minus}#pi^{+}#pi^{#minus}";
        }
    }

    // Return
    return leg;
}
