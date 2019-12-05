#include "TCanvas.h"
#include "TFile.h"
#include "TLegend.h"
#include "TPad.h"
#include "TROOT.h"
#include "TTree.h"
#include "TH1F.h"
#include "TLine.h"

#include "RooHist.h"
#include "RooPlot.h"
#include "RooGlobalFunc.h"

#include "PlotStyle.hpp"

#include "Plotter.hpp"


// ===================
// Default constructor
// ===================
Plotter::Plotter() {}


// ===========
// Constructor
// ===========
Plotter::Plotter(std::string mode, std::string type) {
    m_mode = mode;
    m_type = type;
    m_path = "/home/pullen/analysis/B02DKstar/Fit_monte_carlo/Histograms/" + type
        + "_" + mode + ".root";
}


// ==========
// Destructor
// ==========
Plotter::~Plotter() {}


// ======================================================
// General plotting function (plots data, fit, and pulls)
// ======================================================
void Plotter::plotFit() {
    // Set custom plot style options
    setPlotStyle();

    // Get histogram file
    TFile* file = TFile::Open(m_path.c_str(), "READ");
    gROOT->ForceStyle();

    // Get histograms
    TH1F* hData = (TH1F*)file->Get("data");
    TH1F* hFit = (TH1F*)file->Get("fit");
    RooHist* hPull = (RooHist*)file->Get("pulls");

    // Make canvas
    TCanvas* canvas = new TCanvas("Bd_M", "", 900, 900);
    TPad* pad1 = new TPad("Fit", "", 0.0, 1.0/3.0, 1.0, 1.0);
    pad1->cd();

    // Draw data points
    hData->SetXTitle("M(D(K#pi)K^{*0})(MeV/c^{2})");
    hData->GetYaxis()->SetTitle("Candidates");
    hData->SetLineColor(kBlack);
    hData->SetLineWidth(1);
    hData->SetMarkerSize(0);
    hData->SetStats(kFALSE);
    hData->Draw("E");

    // Draw fit
    hFit->SetLineColor(ANABlue);
    hFit->SetMarkerColor(ANABlue);
    hFit->SetMarkerStyle(0);
    hFit->Draw("HIST C SAME");
    hData->Draw("E SAME");
    
    // Add legend
    TLegend* leg; 
    if (m_type.find("DKpipi") != std::string::npos) {
        if (m_type.find("DKpipi_2") != std::string::npos) {
            leg = new TLegend(0.6, 0.75, 0.9, 0.9);
        } else {
            leg = new TLegend(0.2, 0.2, 0.5, 0.5);
        }
    } else {
        leg = new TLegend(0.2, 0.75, 0.5, 0.9);
    }
    leg->AddEntry(hData, "Monte Carlo");
    leg->AddEntry(hFit, "Fit");
    leg->SetFillStyle(0);
    leg->Draw();

    // Get min/max mass for pull plot
    double xMin = hData->GetXaxis()->GetXmin();
    double xMax = hData->GetXaxis()->GetXmax();

    // Draw pulls
    TPad* pad2 = new TPad("Pulls", "", 0.0, 0.0, 1.0, 1.0/3.0);
    pad2->cd();
    RooPlot* frame = new RooPlot(xMin, xMax);
    frame->SetMinimum(-5);
    frame->SetMaximum(5);
    hPull->SetFillColor(kBlue + 3);
    frame->addPlotable(hPull, "BEX0");
    frame->Draw();
    TLine * line = new TLine(xMin, -3, xMax, -3);
    line->SetLineStyle(2);
    line->SetLineColor(kRed + 2);
    TLine * line2 = new TLine(xMax, 3, xMin, 3);
    line2->SetLineStyle(2);
    line2->SetLineColor(kRed + 2);
    if (m_type.find("DKpipi") == std::string::npos) {
        line->Draw();
        line2->Draw();
    }

    // Save
    canvas->cd();
    pad1->Draw();
    pad2->Draw();
    std::string outname = "../Plots/" + m_type + "_" + m_mode + ".pdf";
    canvas->SaveAs(outname.c_str());

    // // Make log version
    // if (m_type.find("DKpipi") == std::string::npos) {
        // canvas->Clear();
        // pad1->SetLogy();
        // pad1->Draw();
        // pad2->Draw();
        // outname = "../Plots/" + m_type + "_" + m_mode + "_log.pdf";
        // std::cout << "Plotter: made a log plot!" << std::endl;
        // canvas->SaveAs(outname.c_str());
    // }
    
    // Make canvas without pulls
    // TCanvas * canvas2 = new TCanvas("nopulls", "", 500, 400);
    // canvas2->cd();
    // std::string outname2 = "../Plots/" + m_type + "_" + m_mode + "_noPull.pdf";
    // hData->Draw("E");
    // hFit->Draw("C SAME");
    // hData->Draw("E SAME");
    // leg->Draw();
    // canvas2->SaveAs(outname2.c_str());

    // Clean up
    delete hData;
    delete hFit;
    delete leg;
    delete canvas;
    // delete canvas2;
}

// ====================================================
// Overloaded plotFit function to plot extra components
// ====================================================
void Plotter::plotFit(std::string comp1, std::string comp2) {
    // Set custom plot style options
    setPlotStyle();

    // Get histogram file
    TFile* file = TFile::Open(m_path.c_str(), "READ");
    gROOT->ForceStyle();

    // Get histograms
    TH1F* hData = (TH1F*)file->Get("data");
    TH1F* hFit = (TH1F*)file->Get("fit");
    TH1F* hComp1 = (TH1F*)file->Get(comp1.c_str());
    TH1F* hComp2 = (TH1F*)file->Get(comp2.c_str());
    RooHist* hPull = (RooHist*)file->Get("pulls");

    // Make canvas
    TCanvas* canvas = new TCanvas("Bd_M", "", 900, 900);
    TPad* pad1 = new TPad("Fit", "", 0.0, 1.0/3.0, 1.0, 1.0);
    pad1->cd();

    // Find bin width for axis label
    int binSize = hData->GetXaxis()->GetBinWidth(1);

    // Get formatted string for mode
    std::string mode_math = m_mode;
    if (m_mode == "Kpi" || m_mode == "Bs" || m_mode == "B0") mode_math = "K#pi";
    else if (m_mode == "pipi") mode_math = "#pi#pi";
    else if (m_mode == "Kpipipi") mode_math = "K#pi#pi#pi";
    else if (m_mode == "pipipipi") mode_math = "pipipipi";

    // Draw data points
    hData->SetXTitle(("M(D(" + mode_math + ")K^{*0})(MeV/c^{2})").c_str());
    hData->GetYaxis()->SetTitle(("Candidates / (" + std::to_string(binSize) +
                " MeV/#it{c}^{2})").c_str());
    hData->SetLineColor(kBlack);
    hData->SetLineWidth(1);
    hData->SetMarkerSize(0);
    hData->SetStats(kFALSE);
    hData->Draw("E");

    // Draw components
    hComp2->SetLineColor(ANAGreen);
    hComp2->SetMarkerColor(ANAGreen);
    hComp2->SetMarkerStyle(0);
    hComp2->SetLineWidth(1);
    hComp2->Draw("HIST C SAME");
    hComp1->SetLineColor(kRed + 2);
    hComp1->SetMarkerColor(kRed + 2);
    hComp1->SetMarkerStyle(0);
    hComp1->SetLineWidth(1);
    hComp1->Draw("HIST C SAME");

    // Draw fit
    hFit->SetLineColor(kBlack);
    hFit->SetMarkerColor(kBlack);
    hFit->SetMarkerStyle(0);
    hFit->SetLineWidth(2);
    hFit->Draw("HIST C SAME");
    hData->Draw("E SAME");
    
    // Add legend
    TLegend* leg = new TLegend(0.6, 0.75, 0.9, 0.9);
    leg->AddEntry(hData, "Monte Carlo");
    leg->AddEntry(hFit, "Fit");
    leg->SetFillStyle(0);
    leg->Draw();

    // Get min/max mass for pull plot
    double xMin = hData->GetXaxis()->GetXmin();
    double xMax = hData->GetXaxis()->GetXmax();

    // Draw pulls
    TPad* pad2 = new TPad("Pulls", "", 0.0, 0.0, 1.0, 1.0/3.0);
    pad2->cd();
    RooPlot* frame = new RooPlot(xMin, xMax);
    frame->SetMinimum(-5);
    frame->SetMaximum(5);
    hPull->SetFillColor(kBlue + 3);
    frame->addPlotable(hPull, "BEX0");
    frame->Draw();
    double line_min = hData->GetXaxis()->GetBinLowEdge(1);
    double line_max = hData->GetXaxis()->GetBinUpEdge(hData->GetXaxis()->GetLast());
    TLine * line = new TLine(line_min, -3, line_max, -3);
    line->SetLineStyle(2);
    line->SetLineColor(kRed + 2);
    TLine * line2 = new TLine(line_min, 3, line_max, 3);
    line2->SetLineStyle(2);
    line2->SetLineColor(kRed + 2);
    if (m_type.find("DKpipi") == std::string::npos) {
        line->Draw();
        line2->Draw();
    }

    // Save
    canvas->cd();
    pad1->Draw();
    pad2->Draw();
    std::string outname = "../Plots/" + m_type + "_" + m_mode + ".pdf";
    canvas->SaveAs(outname.c_str());

    // Make log version
    TCanvas* canvas_log = new TCanvas("Bd_M_log", "", 900, 900);
    TPad* pad1_log = new TPad("Fit_log", "", 0.0, 1.0/3.0, 1.0, 1.0);
    pad1_log->SetLogy();
    pad1_log->cd();
    hData->Draw("E");
    hComp1->Draw("HIST C SAME");
    hComp2->Draw("HIST C SAME");
    hFit->Draw("HIST C SAME");
    hData->Draw("E SAME");
    leg->Draw();
    canvas_log->cd();
    pad1_log->Draw();
    pad2->Draw();
    std::string outname_log = "../Plots/" + m_type + "_" + m_mode + "_log.pdf";
    canvas_log->SaveAs(outname_log.c_str());

    // Clean up
    delete hData;
    delete hFit;
    delete hComp1;
    delete hComp2;
    delete leg;
    delete canvas;
}


// ====================================================
// Overloaded plotFit function to plot extra components
// ====================================================
void Plotter::plotFit(std::string comp1, std::string comp2, std::string comp3) {
    // Set custom plot style options
    setPlotStyle();

    // Get histogram file
    TFile* file = TFile::Open(m_path.c_str(), "READ");
    gROOT->ForceStyle();

    // Get histograms
    TH1F* hData = (TH1F*)file->Get("data");
    TH1F* hFit = (TH1F*)file->Get("fit");
    TH1F* hComp1 = (TH1F*)file->Get(comp1.c_str());
    TH1F* hComp2 = (TH1F*)file->Get(comp2.c_str());
    TH1F* hComp3 = (TH1F*)file->Get(comp3.c_str());
    RooHist* hPull = (RooHist*)file->Get("pulls");

    // Make canvas
    TCanvas* canvas = new TCanvas("Bd_M", "", 900, 900);
    TPad* pad1 = new TPad("Fit", "", 0.0, 1.0/3.0, 1.0, 1.0);
    pad1->cd();

    // Find bin width for axis label
    int binSize = hData->GetXaxis()->GetBinWidth(1);

    // Get formatted string for mode
    std::string mode_math = m_mode;
    if (m_mode == "Kpi" || m_mode == "Bs") mode_math = "K#pi";
    else if (m_mode == "pipi") mode_math = "#pi#pi";
    else if (m_mode == "Kpipipi") mode_math = "K#pi#pi#pi";
    else if (m_mode == "pipipipi") mode_math = "pipipipi";

    // Draw data points
    hData->SetXTitle(("M(D(" + mode_math + ")K^{*0})(MeV/c^{2})").c_str());
    hData->GetYaxis()->SetTitle(("Candidates / (" + std::to_string(binSize) +
                " MeV/#it{c}^{2})").c_str());
    hData->SetLineColor(kBlack);
    hData->SetLineWidth(1);
    hData->SetMarkerSize(0);
    hData->SetStats(kFALSE);
    hData->Draw("E");

    // Draw components
    std::cout << "Drawing component 1: " << comp1 << std::endl;
    hComp1->SetLineColor(kRed + 2);
    hComp1->SetMarkerColor(kRed + 2);
    hComp1->SetMarkerStyle(0);
    hComp1->SetLineWidth(1);
    hComp1->Draw("C SAME");
    std::cout << "Drawing component 2: " << comp2 << std::endl;
    hComp2->SetLineColor(ANAGreen);
    hComp2->SetMarkerColor(ANAGreen);
    hComp2->SetMarkerStyle(0);
    hComp2->SetLineWidth(1);
    hComp2->Draw("C SAME");
    std::cout << "Drawing component 3: " << comp3 << std::endl;
    hComp3->SetLineColor(ANABlue);
    hComp3->SetMarkerColor(ANABlue);
    hComp3->SetMarkerStyle(0);
    hComp3->SetLineWidth(1);
    hComp3->Draw("C SAME");

    // Draw fit
    hFit->SetLineColor(kBlack);
    hFit->SetMarkerColor(kBlack);
    hFit->SetMarkerStyle(0);
    hFit->SetLineWidth(2);
    hFit->Draw("C SAME");
    hData->Draw("E SAME");
    
    // Add legend
    TLegend* leg = new TLegend(0.6, 0.75, 0.9, 0.9);
    leg->AddEntry(hData, "Monte Carlo");
    leg->AddEntry(hFit, "Fit");
    leg->SetFillStyle(0);
    leg->Draw();

    // Get min/max mass for pull plot
    double xMin = hData->GetXaxis()->GetXmin();
    double xMax = hData->GetXaxis()->GetXmax();

    // Draw pulls
    TPad* pad2 = new TPad("Pulls", "", 0.0, 0.0, 1.0, 1.0/3.0);
    pad2->cd();
    RooPlot* frame = new RooPlot(xMin, xMax);
    frame->SetMinimum(-5);
    frame->SetMaximum(5);
    hPull->SetFillColor(kBlue + 3);
    frame->addPlotable(hPull, "BEX0");
    frame->Draw();
    double line_min = hData->GetXaxis()->GetBinLowEdge(1);
    double line_max = hData->GetXaxis()->GetBinUpEdge(hData->GetXaxis()->GetLast());
    TLine * line = new TLine(line_min, -3, line_max, -3);
    line->SetLineStyle(2);
    line->SetLineColor(kRed + 2);
    TLine * line2 = new TLine(line_min, 3, line_max, 3);
    line2->SetLineStyle(2);
    line2->SetLineColor(kRed + 2);

    // Save
    canvas->cd();
    pad1->Draw();
    pad2->Draw();
    std::string outname = "../Plots/" + m_type + "_" + m_mode + ".pdf";
    canvas->SaveAs(outname.c_str());

    // Make log version
    TCanvas* canvas_log = new TCanvas("Bd_M_log", "", 900, 900);
    TPad* pad1_log = new TPad("Fit_log", "", 0.0, 1.0/3.0, 1.0, 1.0);
    pad1_log->GetLogy();
    pad1_log->cd();
    hData->Draw("E");
    hComp1->Draw("C SAME");
    hComp2->Draw("C SAME");
    hComp3->Draw("C SAME");
    hFit->Draw("C SAME");
    hData->Draw("E SAME");
    leg->Draw();
    canvas_log->cd();
    pad1_log->Draw();
    pad2->Draw();
    std::string outname_log = "../Plots/" + m_type + "_" + m_mode + "_log.pdf";
    canvas_log->SaveAs(outname_log.c_str());

    // Clean up
    delete hData;
    delete hFit;
    delete hComp1;
    delete hComp2;
    delete leg;
    delete canvas;
}



// ==================================
// Plot the fits for low mass shapes
// ==================================
void Plotter::plotLowMassFits() {

    // Plot each fit
    plotSingleLowMassComponent("gamma_010");
    plotSingleLowMassComponent("gamma_101");
    plotSingleLowMassComponent("pi_010");
    plotSingleLowMassComponent("pi_101");
    plotSingleLowMassComponent("Bs_gamma_010");
    plotSingleLowMassComponent("Bs_gamma_101");
    plotSingleLowMassComponent("Bs_pi_010");
    plotSingleLowMassComponent("Bs_pi_101");

}


// ============================
// Plot a single low mass shape
// ============================
void Plotter::plotSingleLowMassComponent(std::string comp) {

    // Set custom plotting style
    setPlotStyle();

    // Get histogram file
    std::string filename = "../Histograms/lowMass.root";
    TFile * file = TFile::Open(filename.c_str(), "READ");
    gROOT->ForceStyle();

    // Get histograms
    TH1F * hData = (TH1F*)file->Get(("data_" + comp).c_str());
    TH1F * hFit = (TH1F*)file->Get(("fit_" + comp).c_str());
    // RooHist * hPull = (RooHist*)file->Get(("pulls_" + comp).c_str());

    // Make canvas
    TCanvas* canvas = new TCanvas("Bd_M", "", 900, 600);
    // TPad* pad1 = new TPad("Fit", "", 0.0, 1.0/3.0, 1.0, 1.0);
    // pad1->cd();

    // Draw data points
    hData->SetXTitle("M([K#pi]_{D}K^{*0})[MeV/#it{c}^{2}]");
    hData->GetYaxis()->SetTitle("Candidates / (6 MeV/#it{c}^{2})");
    hData->SetLineColor(kBlack);
    hData->SetLineWidth(1);
    hData->SetMarkerSize(0);
    hData->SetStats(kFALSE);
    //hData->GetXaxis()->SetRangeUser(4809, 5399);
    hData->GetXaxis()->SetLabelSize(0.06);
    hData->GetXaxis()->SetTitleOffset(1.1);
    hData->GetYaxis()->SetLabelSize(0.06);
    hData->Draw("E");

    // Draw fit
    hFit->SetLineColor(ANABlue);
    hFit->SetLineWidth(2);
    hFit->SetMarkerColor(ANABlue);
    hFit->SetMarkerStyle(0);
    hFit->Draw("C SAME");
    hData->Draw("E SAME");
    
    // Add legend
    TLegend* leg; 
    if (m_type.find("DKpipi") != std::string::npos) {
        leg = new TLegend(0.6, 0.75, 0.9, 0.9);
    } else {
        leg = new TLegend(0.2, 0.75, 0.5, 0.9);
    }
    leg->AddEntry(hData, "Monte Carlo");
    leg->AddEntry(hFit, "Fit");
    leg->SetFillStyle(0);
    leg->Draw();

    // Draw pulls
    // TPad * pad2 = new TPad("Pulls", "", 0.0, 0.0, 1.0, 1.0/3.0);
    // pad2->cd();
    // RooPlot * frame = new RooPlot(hData->GetXaxis()->GetXmin(),
            // hData->GetXaxis()->GetXmax());
    // frame->SetMinimum(-5);
    // frame->SetMaximum(5);
    // hPull->SetFillColor(kBlue + 3);
    // frame->addPlotable(hPull, "BEX0");
    // frame->Draw();
    // TLine * line = new TLine(4800, -3, 5400, -3);
    // line->SetLineStyle(2);
    // line->SetLineColor(kRed + 2);
    // TLine * line2 = new TLine(4800, 3, 5400, 3);
    // line2->SetLineStyle(2);
    // line2->SetLineColor(kRed + 2);
    // if (m_type.find("DKpipi") == std::string::npos) {
        // line->Draw();
        // line2->Draw();
    // }

    // Save
    canvas->cd();
    // pad1->Draw();
    // pad2->Draw();
    std::string outname = "../Plots/lowMass_" + comp + ".pdf";
    canvas->SaveAs(outname.c_str());
    delete hData;
    delete hFit;
    delete canvas;
}


// =================================================================
// Overloaded plotFit function to plot arbitary number of components
// =================================================================
void Plotter::plotFit(std::vector<std::string> components) {

    // Set custom plot style options
    setPlotStyle();

    // Get histogram file
    TFile* file = TFile::Open(m_path.c_str(), "READ");
    gROOT->ForceStyle();

    // Get histograms
    TH1F* hData = (TH1F*)file->Get("data");
    TH1F* hFit = (TH1F*)file->Get("fit");
    RooHist* hPull = (RooHist*)file->Get("pulls");
    std::map<std::string, TH1F*> comps;
    for (auto comp : components) {
        comps[comp] = (TH1F*)file->Get(comp.c_str());
    }

    // Make canvas
    TCanvas* canvas = new TCanvas("Bd_M", "", 900, 900);
    TPad* pad1 = new TPad("Fit", "", 0.0, 1.0/3.0, 1.0, 1.0);
    pad1->cd();

    // Find bin width for axis label
    int binSize = hData->GetXaxis()->GetBinWidth(1);

    // Get formatted string for mode
    std::string mode_math = m_mode;
    if (m_mode == "Kpi" || m_mode == "Bs" || m_mode == "B0") mode_math = "K#pi";
    else if (m_mode == "pipi") mode_math = "#pi#pi";
    else if (m_mode == "Kpipipi") mode_math = "K#pi#pi#pi";
    else if (m_mode == "pipipipi") mode_math = "pipipipi";

    // Draw data points
    hData->SetXTitle(("M(D(" + mode_math + ")K^{*0})(MeV/c^{2})").c_str());
    hData->GetYaxis()->SetTitle(("Candidates / (" + std::to_string(binSize) +
                " MeV/#it{c}^{2})").c_str());
    hData->SetLineColor(kBlack);
    hData->SetLineWidth(1);
    hData->SetMarkerSize(0);
    hData->SetStats(kFALSE);
    hData->Draw("E");

    // Draw components
    int color = 1;
    for (auto c : comps) {
        c.second->SetLineColor(color);
        c.second->SetLineWidth(1);
        c.second->Draw("HIST C SAME");
        color++;
    }

    // Draw fit
    hFit->SetLineColor(kBlack);
    hFit->SetMarkerColor(kBlack);
    hFit->SetMarkerStyle(0);
    hFit->SetLineWidth(2);
    hFit->Draw("HIST C SAME");
    hData->Draw("E SAME");
    
    // Add legend
    TLegend* leg = new TLegend(0.6, 0.75, 0.9, 0.9);
    leg->AddEntry(hData, "Monte Carlo");
    leg->AddEntry(hFit, "Fit");
    leg->SetFillStyle(0);
    leg->Draw();

    // Get min/max mass for pull plot
    double xMin = hData->GetXaxis()->GetXmin();
    double xMax = hData->GetXaxis()->GetXmax();

    // Draw pulls
    TPad* pad2 = new TPad("Pulls", "", 0.0, 0.0, 1.0, 1.0/3.0);
    pad2->cd();
    RooPlot* frame = new RooPlot(xMin, xMax);
    frame->SetMinimum(-5);
    frame->SetMaximum(5);
    hPull->SetFillColor(kBlue + 3);
    frame->addPlotable(hPull, "BEX0");
    frame->Draw();
    double line_min = hData->GetXaxis()->GetBinLowEdge(1);
    double line_max = hData->GetXaxis()->GetBinUpEdge(hData->GetXaxis()->GetLast());
    TLine * line = new TLine(line_min, -3, line_max, -3);
    line->SetLineStyle(2);
    line->SetLineColor(kRed + 2);
    TLine * line2 = new TLine(line_min, 3, line_max, 3);
    line2->SetLineStyle(2);
    line2->SetLineColor(kRed + 2);
    if (m_type.find("DKpipi") == std::string::npos) {
        line->Draw();
        line2->Draw();
    }

    // Save
    canvas->cd();
    pad1->Draw();
    pad2->Draw();
    std::string outname = "../Plots/" + m_type + "_" + m_mode + ".pdf";
    canvas->SaveAs(outname.c_str());

    // Make log version
    TCanvas* canvas_log = new TCanvas("Bd_M_log", "", 900, 900);
    TPad* pad1_log = new TPad("Fit_log", "", 0.0, 1.0/3.0, 1.0, 1.0);
    pad1_log->SetLogy();
    pad1_log->cd();
    hData->Draw("E");
    for (auto c : comps) {
        c.second->Draw("HIST C SAME");
    }
    hFit->Draw("HIST C SAME");
    hData->Draw("E SAME");
    leg->Draw();
    canvas_log->cd();
    pad1_log->Draw();
    pad2->Draw();
    std::string outname_log = "../Plots/" + m_type + "_" + m_mode + "_log.pdf";
    canvas_log->SaveAs(outname_log.c_str());

    // Clean up
    delete hData;
    delete hFit;
    delete leg;
    delete canvas;
}

