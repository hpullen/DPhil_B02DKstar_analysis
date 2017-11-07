// System includes
#include <iostream>

// ROOT includes
#include "TCanvas.h"
#include "TFile.h"
#include "TH1F.h"
#include "TLegend.h"
#include "TPad.h"
#include "TLine.h"
#include "TROOT.h"
#include "THStack.h"
#include "RooHist.h"
#include "RooPlot.h"
#include "RooRealVar.h"

// Library includes
#include "PlotStyle.hpp"

// Local includes
#include "Plotter.hpp"

// ===========
// Constructor
// ===========
Plotter::Plotter() {
}


// ==========
// Destructor
// ==========
Plotter::~Plotter() {
}


// ===================================
// Plot all fits for Kpi and piK modes
// ===================================
void Plotter::plotTwoModeFits(std::string histofile, std::string plotname, 
        std::string saveDir) {

    // Set current plot name 
    m_current_name = plotname;
    m_current_histfile = histofile;

    // Make and divide canvas
    TCanvas * canvas = new TCanvas("allPlots", "", 800, 400);
    canvas->Divide(2, 2, 0.0005, 0.0005);

    // Plot Kpi fits
    plotKpiFit("matter", canvas, 1, saveDir);
    plotKpiFit("antimatter", canvas, 2, saveDir);

    // Plot piK fits
    plotBlindFit("piK", "matter", canvas, 3, saveDir);
    plotBlindFit("piK", "antimatter", canvas, 4, saveDir);

    // Plot Kpipipi fits

    // Save full canvas
    canvas->SaveAs(("../Plots/" + saveDir + "/" + plotname + ".pdf").c_str());
}


// ========================================
// Plot combined fits for Kpi and piK modes
// ========================================
void Plotter::plotTwoModeFitsCombined(std::string histofile, std::string plotname, 
        std::string saveDir) {

    // Set current plot name
    m_current_name = plotname;
    m_current_histfile = histofile;

    // Make and divide canvas
    TCanvas * canvas = new TCanvas("allPlots", "", 500, 800);
    canvas->Divide(1, 2, 0.0005, 0.0005);

    // Plot Kpi and piK fits
    plotKpiFit("combined", canvas, 1, saveDir);
    plotBlindFit("piK", "combined", canvas, 2, saveDir);

    // Save full canvas
    canvas->SaveAs(("../Plots/" + saveDir + "/" + plotname + 
                "_combined.pdf").c_str());
}


// ======================================================
// Plot combined fits for Kpi, piK, KK, and pipi modes
// ======================================================
void Plotter::plotFourModeFitsCombined(std::string histofile, std::string plotname,
        std::string saveDir) {

    // Set current plotname
    m_current_name = plotname;
    m_current_histfile = histofile;

    // Make and divide canvas
    std::cout << "Making canvas" << std::endl;
    TCanvas * canvas = new TCanvas("allPlots", "", 500, 400);
    std::cout << "Dividing canvas" << std::endl;
    canvas->Divide(2, 2, 0.0005, 0.0005);

    // Plot all fits
    std::cout << "Plotting Kpi" << std::endl;
    plotKpiFit("combined", canvas, 1, saveDir);
    std::cout << "Plotting piK" << std::endl;
    plotBlindFit("piK", "combined", canvas, 2, saveDir);
    plotBlindFit("KK", "combined", canvas, 3, saveDir);
    plotBlindFit("pipi", "combined", canvas, 4, saveDir);

    // Save full canvas
    canvas->SaveAs(("../Plots/" + saveDir + "/" + plotname + 
                "_combined_allmodes.pdf").c_str());
    plotKpiFit("Kpipipi_combined", canvas, 1, saveDir);
    plotBlindFit("piKpipi", "combined", canvas, 2, saveDir);
    plotBlindFit("pipipipi", "combined", canvas, 2, saveDir);
}


// ===========================================================
// Plot flavour separated fits for Kpi, piK, KK and pipi modes
// ===========================================================
void Plotter::plotFourModeFitsSeparate(std::string histofile, std::string plotname,
        std::string saveDir) {

    // Set current plotname
    m_current_name = plotname;
    m_current_histfile = histofile;

    // Make and divide canvas
    TCanvas * canvas1 = new TCanvas("allPlots_matter_Kpi", "", 500, 400);
    canvas1->Divide(2, 2, 0.0005, 0.0005);
    TCanvas * canvas2 = new TCanvas("allPlots_matter_KK", "", 500, 400);
    canvas2->Divide(2, 2, 0.0005, 0.0005);

    // Plot all fits
    // Kpi and piK
    plotKpiFit("", canvas1, 1, saveDir);
    plotKpiFit("bar", canvas1, 2, saveDir);
    plotBlindFit("piK", "", canvas1, 3, saveDir);
    plotBlindFit("piK", "bar", canvas1, 4, saveDir);

    // KK and pipi
    plotBlindFit("KK", "", canvas2, 1, saveDir);
    plotBlindFit("KK", "bar", canvas2, 2, saveDir);
    plotBlindFit("pipi", "", canvas2, 3, saveDir);
    plotBlindFit("pipi", "bar", canvas2, 4, saveDir);

    // Save full canvas
    canvas1->SaveAs(("../Plots/" + saveDir + "/" + plotname + 
                "_flavourSplit_Kpi_piK.pdf").c_str());
    canvas2->SaveAs(("../Plots/" + saveDir + "/" + plotname + 
                "_flavourSplit_KK_pipi.pdf").c_str());

    // Plot four body fit
    plotKpiFit("Kpipipi", canvas2, 1, saveDir); 
    plotKpiFit("Kpipipi_bar", canvas2, 1, saveDir); 
    plotBlindFit("piKpipi", "", canvas1, 3, saveDir);
    plotBlindFit("piKpipi", "bar", canvas1, 4, saveDir);
    plotBlindFit("pipipipi", "", canvas2, 3, saveDir);
    plotBlindFit("pipipipi", "bar", canvas2, 4, saveDir);

}


// ===============================
// Plot Kpi fit (no Bs components)
// ===============================
void Plotter::plotKpiFit(std::string flav, TCanvas * all_canvas, int canvas_number, 
        std::string saveDir) 
{
    
    // Setup plotting style
    setPlotStyle();
    bool combFit = false;
    std::string name_extra;
    if (flav == "Kpipipi") {
        flav = "pipi";
        name_extra = flav;
    } else if (flav == "Kpipipi_bar") {
        flav = "pipi_bar";
        name_extra = flav;
    } else if (flav == "Kpipipi_combined") {
        combFit = true;
        name_extra = "pipi_combined";
        flav = "pipi";
    } else {
        if (flav != "") {
            flav = "_" + flav;
        }
        name_extra = flav;
        if (flav == "_combined") {
            combFit = true;
            flav = "";
        }
    }

    // Open file 
    TFile* file = TFile::Open(m_current_histfile.c_str(), "READ");
    gROOT->ForceStyle();

    // Read in histograms
    TH1F* hData = (TH1F*)file->Get(("data_Kpi" + flav).c_str());
    TH1F* hFit = (TH1F*)file->Get(("fit_Kpi" + flav).c_str());
    TH1F* hGauss = (TH1F*)file->Get(("signal_Kpi" + flav).c_str());
    TH1F* hExpo = (TH1F*)file->Get(("expo_Kpi" + flav).c_str());
    TH1F* hLow = (TH1F*)file->Get(("low_Kpi" + flav).c_str());
    RooHist* hPull = (RooHist*)file->Get(("pulls_Kpi" + flav).c_str());

    // Make canvas for full plot
    TCanvas* canvas = new TCanvas("", "", 1.25 * 700, 1000);
    TPad* pad1 = new TPad("Fit", "", 0.0, 0.3, 1.0, 1.0);
    pad1->cd();

    // Set style of datapoints
    gROOT->ForceStyle();
	hData->SetLineColor(kBlack);
	hData->SetLineWidth(1);
    hData->SetMarkerSize(0);
    hData->SetMarkerStyle(0);
    hData->SetStats(kFALSE);
    std::string Kstar_math = "K^{*0}";
    if (flav.find("_bar") != std::string::npos) {
        Kstar_math = "#bar{K}^{*0}";
    }
    if (flav.find("pipi") != std::string::npos) {
        hData->SetXTitle(("#it{m}([K#pi#pi#pi]_{D}" + Kstar_math + ") [MeV/#it{c}^{2}]").c_str());
    } else {
        hData->SetXTitle(("#it{m}([K#pi]_{D}" + Kstar_math + ") [MeV/#it{c}^{2}]").c_str());
    }
    hData->GetYaxis()->SetTitle("Candidates / (10 MeV/#it{c}^{2})");
    hData->GetXaxis()->SetTitleOffset(1.15);
    hData->GetYaxis()->SetTitleOffset(1.1);
    hData->GetXaxis()->SetLabelSize(0.06);
    hData->GetYaxis()->SetLabelSize(0.06);
    hData->GetXaxis()->SetRangeUser(5009, 5799);

    // Set style of fit
    hFit->SetLineColor(kBlack);
    hFit->SetMarkerColor(kBlack);
    hFit->SetMarkerStyle(0);
    hFit->SetStats(kFALSE);
    hFit->SetXTitle(("#it{m}([K#pi]_{D}" + Kstar_math + ") [MeV/#it{c}^{2}]").c_str());
    hFit->GetYaxis()->SetTitle("Candidates / (10 MeV/#it{c}^{2})");
    hFit->GetXaxis()->SetTitleOffset(1.15);
    hFit->GetYaxis()->SetTitleOffset(1.1);
    hFit->GetXaxis()->SetLabelSize(0.06);
    hFit->GetYaxis()->SetLabelSize(0.06);
    hFit->GetXaxis()->SetRangeUser(5009, 5799);

    // Fix range if not a combined fit
    if (!combFit) {
        //hFit->GetYaxis()->SetRangeUser(0, 199);
        //hData->GetYaxis()->SetRangeUser(0, 199);
    } else {
        //hFit->GetYaxis()->SetRangeUser(0, 349);
        //hData->GetYaxis()->SetRangeUser(0, 349);
    }

    // Draw whichever is tallest first
    if (hFit->GetMaximum() > hData->GetMaximum()) {
        hFit->Draw("C");
        hData->Draw("E SAME");
    } else {
        hData->Draw("E");
        hFit->Draw("C SAME");
    }
    
    // Draw backgrounds in stack
	THStack* hStack = new THStack("hStack", "");
    hExpo->SetLineColor(ANABlue);
    hExpo->SetMarkerColor(ANABlue);
    hExpo->SetMarkerStyle(0);
    hExpo->SetFillColor(ANABlue);
    hExpo->SetLineWidth(0);
    hStack->Add(hExpo);
    hLow->SetLineColor(kOrange);
    hLow->SetMarkerColor(kOrange);
    hLow->SetMarkerStyle(0);
    hLow->SetFillColor(kOrange);
    hLow->SetLineWidth(0);
    hStack->Add(hLow);
    // hRho->SetLineColor(ANAPurple);
    // hRho->SetMarkerColor(ANAPurple);
    // hRho->SetMarkerStyle(0);
    // hRho->SetFillColor(ANAPurple);
    // hRho->SetLineWidth(0);
    // hStack->Add(hRho);
    hStack->Draw("C HIST SAME");

    // Plot overall fit and signal
    hGauss->SetLineColor(kRed + 1);
    hGauss->SetMarkerColor(kRed + 1);
    hGauss->SetMarkerStyle(0);
    hGauss->SetFillStyle(0);
    hGauss->Draw("C HIST SAME");

    // Draw datapoints on top
    hFit->Draw("C SAME");
    hData->Draw("E SAME");

    // Make a legend
    TLegend* leg = new TLegend(0.55, 0.45, 0.85, 0.9);
    leg->SetFillStyle(0);
    leg->AddEntry(hData, "Data");
    leg->AddEntry(hFit, "Fit");
    if (flav == "matter" || flav == "" || flav == "pipi") {
        leg->AddEntry(hGauss, "B^{0}_{d}#rightarrowDK^{*0}");
        leg->AddEntry(hExpo, "Combinatorial");
        leg->AddEntry(hLow, "B^{0}_{d}#rightarrowD^{*}K^{*0}");
        // leg->AddEntry(hRho, "B^{0}_{d}#rightarrowD#rho^{0}");
    } else if (flav == "antimatter" || flav == "_bar" || flav == "pipi_bar") {
        leg->AddEntry(hGauss, "#bar{B}^{0}_{d}#rightarrowD#bar{K}^{*0}");
        leg->AddEntry(hExpo, "Combinatorial");
        leg->AddEntry(hLow, "#bar{B}^{0}_{d}#rightarrowD^{*}#bar{K}^{*0}");
        // leg->AddEntry(hRho, "B^{0}_{d}#rightarrowD#rho^{0}");
    }
    leg->Draw();
    gPad->RedrawAxis();

    // Draw pulls
    TPad* pad2 = new TPad("Pulls", "", 0.0, 0.0, 1.0, 0.3);
    pad2->cd();
    RooPlot* frame = new RooPlot(5000, 5800);
    hPull->SetFillColor(kBlue + 3);
    frame->SetMinimum(-5);
    frame->SetMaximum(5);
    frame->addPlotable(hPull, "BEX0");
    frame->Draw();
    TLine * line = new TLine(5000, -3, 5800, -3);
    line->SetLineStyle(2);
    line->SetLineColor(kRed + 2);
    line->Draw();
    TLine * line2 = new TLine(5000, 3, 5800, 3);
    line2->SetLineStyle(2);
    line2->SetLineColor(kRed + 2);
    line2->Draw();
    canvas->cd();
    pad1->Draw();
    pad2->Draw();

    // Save plot
    std::string outname_fit = "../Plots/" + saveDir + "/" + m_current_name + 
        "_Kpi" + name_extra + ".pdf";
    canvas->SaveAs(outname_fit.c_str());

    // Save without pulls
    TCanvas * canv_nopull = new TCanvas("nopull", "", 500, 400);
    canv_nopull->cd();
    pad1->SetPad(0.0, 0.0, 1.0, 1.0);
    pad1->Draw();
    canv_nopull->SaveAs(("../Plots/" + saveDir + "/noPulls/" + m_current_name + 
                "_Kpi" + name_extra + ".pdf").c_str());

    // Plot on big canvas
    all_canvas->cd(canvas_number);
    hFit->GetXaxis()->SetTitle("");
    hFit->GetYaxis()->SetTitle("");
    hFit->SetLineWidth(1);
    hData->GetXaxis()->SetTitle("");
    hData->GetYaxis()->SetTitle("");
    if (hFit->GetMaximum() > hData->GetMaximum()) {
        hFit->Draw("C");
        hData->Draw("E SAME");
    } else {
        hData->Draw("E");
        hFit->Draw("C SAME");
    }
    hStack->Draw("C HIST SAME");
    hGauss->SetLineWidth(1);
    hGauss->Draw("C HIST SAME");
    hFit->Draw("C SAME");
    hData->Draw("E SAME");
    gPad->RedrawAxis();
    leg->Draw("SAME");
    delete canvas;
}


// =================================
// Plot piK fit (including Bs peaks)
// =================================
void Plotter::plotBlindFit(std::string mode, std::string flav, 
        TCanvas * all_canvas, int canvas_number, std::string saveDir) {

    std::cout << "Plotting " + mode + " fit" << std::endl;
    // Setup plotting style
    bool combFit = false;
    setPlotStyle();
    if (flav != "") {
        flav = "_" + flav;
    }
    std::string name_extra = flav;
    if (flav == "_combined") {
        combFit = true;
        flav = "";
    }

    // Get histogram file
    TFile* file = TFile::Open(m_current_histfile.c_str(), "READ");
    gROOT->ForceStyle();

    // Get histograms
    TH1F* hData = (TH1F*)file->Get(("data_" + mode + flav).c_str());
    TH1F* hFit = (TH1F*)file->Get(("fit_" + mode + flav).c_str());
    TH1F* hBs = (TH1F*)file->Get(("Bs_" + mode + flav).c_str());
    TH1F* hExpo = (TH1F*)file->Get(("expo_" + mode + flav).c_str());
    TH1F* hLow = (TH1F*)file->Get(("low_" + mode + flav).c_str());
    TH1F* hLow_Bs = (TH1F*)file->Get(("Bs_low_" + mode + flav).c_str());
    // TH1F* hRho = (TH1F*)file->Get(("rhoBackground_" + mode + flav).c_str());
    RooHist* hPulls_low = (RooHist*)file->Get(("pulls_low_" + mode + flav).c_str());
    RooHist* hPulls_high = (RooHist*)file->Get(("pulls_high_" + mode + 
                flav).c_str());

    // Make new canvas for full plot
    TCanvas* canvas = new TCanvas("", "", 1.25 * 700, 1000);
    TPad* pad1 = new TPad("Fit", "", 0.0, 0.3, 1.0, 1.0);
    pad1->cd();

    // Make axis title
    std::string axisTitle;
    if (mode == "piK") {
        axisTitle = "#it{m}([#piK]_{D}K^{*0}) [MeV/#it{c}^{2}]";
    } else if (mode == "KK") {
        axisTitle = "#it{m}([KK]_{D}K^{*0}) [MeV/#it{c}^{2}]";
    } else if (mode == "pipi") {
        axisTitle = "#it{m}([#pi#pi]_{D}K^{*0}) [MeV/#it{c}^{2}]";
    }; 

    // Set style of datapoints
    gROOT->ForceStyle();
	hData->SetLineColor(kBlack);
	hData->SetLineWidth(1);
    hData->SetMarkerSize(0);
    hData->SetMarkerStyle(8);
    hData->SetStats(kFALSE);
    hData->GetYaxis()->SetTitle("Candidates / (10 MeV/#it{c}^{2})");
    hData->GetXaxis()->SetTitleOffset(1.15);
    hData->GetYaxis()->SetTitleOffset(1.1);
    hData->GetXaxis()->SetLabelSize(0.06);
    hData->GetYaxis()->SetLabelSize(0.06);
    hData->GetXaxis()->SetRangeUser(5009, 5799);

    // Set x title
    std::string mode_math = mode;
    if (mode == "pipi") {
        mode_math = "#pi#pi";
    } else if (mode == "piK") {
        mode_math = "#piK";
    }
    std::string Kstar_math = "K^{*0}";
    if (flav.find("_bar") != std::string::npos) {
        Kstar_math = "#bar{K}^{*0}";
    }
    hData->SetXTitle(("#it{m}([" + mode_math + "]_{D}" + Kstar_math + ") [MeV/#it{c}^{2}]").c_str());
    hFit->SetXTitle(("#it{m}([" + mode_math + "]_{D}" + Kstar_math + ") [MeV/#it{c}^{2}]").c_str());

    // Set style of fit and split
    hFit->SetLineColor(kBlack);
    hFit->SetMarkerColor(kBlack);
    hFit->SetMarkerStyle(0);
    hFit->SetStats(kFALSE);
    hFit->GetYaxis()->SetTitle("Candidates / (10 MeV/#it{c}^{2})");
    hFit->GetXaxis()->SetTitleOffset(1.15);
    hFit->GetYaxis()->SetTitleOffset(1.1);
    hFit->GetXaxis()->SetLabelSize(0.06);
    hFit->GetYaxis()->SetLabelSize(0.06);
    TH1F hFit_low = *hFit;
    hFit_low.GetXaxis()->SetRangeUser(5009, 5279.61 - 50.5);
    TH1F hFit_high = *hFit;
    hFit_high.GetXaxis()->SetRangeUser(5279.61 + 50.5, 5799);

    //// Fix range if not a combined fit
    //if (!combFit) {
        //if (mode == "piK" || mode == "piKpipi") {
            //hFit->GetYaxis()->SetRangeUser(0, 449);
            //hData->GetYaxis()->SetRangeUser(0, 449);
        //} else if (mode == "KK") {
            //hFit->GetYaxis()->SetRangeUser(0, 84);
            //hData->GetYaxis()->SetRangeUser(0, 84);
        //} else if (mode == "pipi") {
            //hFit->GetYaxis()->SetRangeUser(0, 32);
            //hData->GetYaxis()->SetRangeUser(0, 32);
        //}
    //} else {
        //if (mode == "piK" || mode == "piKpipi") {
            //hFit->GetYaxis()->SetRangeUser(0, 799);
            //hData->GetYaxis()->SetRangeUser(0, 799);
        //}
    //}

    // Draw data and fit
    if (hData->GetMaximum() < hFit->GetMaximum()) {
        hData->GetYaxis()->SetRangeUser(0, hFit->GetMaximum() * 1.1);
    }
    hData->Draw("E");
    hFit_low.Draw("C SAME");
    hFit_high.Draw("C SAME");

    // Draw backgrounds in stack
	THStack* hStack = new THStack("hStack", "");
    // Exponential
    hExpo->SetLineColor(ANABlue);
    hExpo->SetMarkerColor(ANABlue);
    hExpo->SetMarkerStyle(0);
    hExpo->SetFillColor(ANABlue);
    hExpo->SetLineStyle(0);
    hExpo->SetLineWidth(0);
    hStack->Add(hExpo);
    // Low mass Bs
    hLow_Bs->SetLineColor(kOrange + 7);
    hLow_Bs->SetMarkerColor(kOrange + 7);
    hLow_Bs->SetMarkerStyle(0);
    hLow_Bs->SetFillColor(kOrange + 7);
    hLow_Bs->SetLineStyle(0);
    hLow_Bs->SetLineWidth(0);
    hStack->Add(hLow_Bs);
    // Low mass Bd
    hLow->SetLineColor(kOrange);
    hLow->SetMarkerColor(kOrange);
    hLow->SetMarkerStyle(0);
    hLow->SetFillColor(kOrange);
    hLow->SetLineStyle(0);
    hLow->SetLineWidth(0);
    hStack->Add(hLow);
    // // Rho0
    // hRho->SetLineColor(ANAPurple);
    // hRho->SetLineStyle(0);
    // hRho->SetMarkerColor(ANAPurple);
    // hRho->SetMarkerStyle(0);
    // hRho->SetFillColor(ANAPurple);
    // hRho->SetLineWidth(0);
    // hStack->Add(hRho);
    // Draw the stack
    hStack->Draw("C HIST SAME");

    // Split and plot Bs peak
    hBs->SetLineColor(ANAGreen);
    hBs->SetMarkerColor(ANAGreen);
    hBs->SetMarkerStyle(0);
    hBs->SetFillStyle(0);
    TH1F hBs_low = *hBs;
    hBs_low.GetXaxis()->SetRangeUser(5000, 5279.61 - 50.01);
    TH1F hBs_high = *hBs;
    hBs_high.GetXaxis()->SetRangeUser(5279.61 + 50.5, 5799);
    hBs_low.Draw("C HIST SAME");
    hBs_high.Draw("C HIST SAME");

    // Redraw datapoints on top
    hFit_low.Draw("C SAME");
    hFit_high.Draw("C SAME");
    hData->Draw("E SAME");

    // Make a legend
    TLegend* leg = new TLegend(0.55, 0.35, 0.85, 0.9);
    leg->SetFillStyle(0);
    leg->AddEntry(hData, "Data");
    leg->AddEntry(hFit, "Fit");
    leg->AddEntry(hExpo, "Combinatorial");
    if (flav == "matter" || flav == "") {
        leg->AddEntry(hBs, "#bar{B}^{0}_{s}#rightarrowDK^{*0}");
        leg->AddEntry(hLow, "B^{0}_{d}#rightarrowD^{*}K^{*0}");
        leg->AddEntry(hLow_Bs, "#bar{B}^{0}_{s}#rightarrowD^{*}K^{*0}");
        // leg->AddEntry(hRho, "B^{0}_{d}#rightarrowD#rho^{0}");
    } else if (flav == "antimatter" || flav == "_bar") {
        leg->AddEntry(hBs, "B^{0}_{s}#rightarrowD#bar{K}^{*0}");
        leg->AddEntry(hLow, "#bar{B}^{0}_{d}#rightarrowD^{*}#bar{K}^{*0}");
        leg->AddEntry(hLow_Bs, "B^{0}_{s}#rightarrowD^{*}#bar{K}^{*0}");
        // leg->AddEntry(hRho, "B^{0}_{d}#rightarrowD#rho^{0}");
    }
    leg->Draw();
    gPad->RedrawAxis();

    // Draw pulls
    TPad* pad2 = new TPad("Pulls", "", 0.0, 0.0, 1.0, 0.3);
    pad2->cd();
    RooPlot* frame = new RooPlot(5000, 5800);
    hPulls_low->SetFillColor(kBlue + 3);
    hPulls_high->SetFillColor(kBlue + 3);
    frame->SetMinimum(-5);
    frame->SetMaximum(5);
    frame->addPlotable(hPulls_low, "BEX0");
    frame->addPlotable(hPulls_high, "BEX0");
    frame->Draw();
    TLine * line = new TLine(5000, -3, 5800, -3);
    line->SetLineStyle(2);
    line->SetLineColor(kRed + 2);
    line->Draw();
    TLine * line2 = new TLine(5000, 3, 5800, 3);
    line2->SetLineStyle(2);
    line2->SetLineColor(kRed + 2);
    line2->Draw();
    canvas->cd();
    pad1->Draw();
    pad2->Draw();

    // Save plot
    std::string outname_fit = "../Plots/" + saveDir + "/"+ m_current_name + "_" + 
        mode + name_extra + ".pdf";
    canvas->SaveAs(outname_fit.c_str());

    // Save without pulls
    TCanvas * canv_nopull = new TCanvas("nopull", "", 500, 400);
    canv_nopull->cd();
    pad1->SetPad(0.0, 0.0, 1.0, 1.0);
    pad1->Draw();
    canv_nopull->SaveAs(("../Plots/" + saveDir + "/noPulls/" + m_current_name + 
                "_" + mode + name_extra + ".pdf").c_str());

    // Plot on big canvas
    all_canvas->cd(canvas_number);
    hFit->SetLineWidth(1);
    hFit->GetXaxis()->SetTitle("");
    hFit->GetYaxis()->SetTitle("");
    hData->GetXaxis()->SetTitle("");
    hData->GetYaxis()->SetTitle("");
    hData->SetLineWidth(1);
    if (hFit->GetMaximum() > hData->GetMaximum()) {
        hFit->Draw("C");
        hData->Draw("E SAME");
    } else {
        hData->Draw("E");
        hFit->Draw("C SAME");
    }
    hStack->Draw("C HIST SAME");
    hBs->SetLineWidth(1);
    hBs->Draw("C HIST SAME");
    hData->Draw("E SAME");
    hFit->Draw("C SAME");
    gPad->RedrawAxis();
    leg->Draw();
    delete canvas;
}
