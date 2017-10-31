#include <iostream>

#include "TLatex.h"
#include "TROOT.h"
#include "TColor.h"
#include "TStyle.h"
#include "TPaveText.h"

#include "PlotStyle.hpp"

// ================================
// Set up custom plot style in ROOT
// ================================
void setPlotStyle() {
    std::cout << "Loaded custom plotting style." << std::endl;

    // No grey background on plots
    gROOT->SetStyle("Plain");

    // Stats box
    gStyle->SetStatStyle(0);
    gStyle->SetStatBorderSize(0);
    gStyle->SetStatH(0.2);
    gStyle->SetStatW(0.225);
    gStyle->SetStatX(0.9);
    gStyle->SetStatY(0.9);
    gStyle->SetStatFormat("4.2g");

    gStyle->SetTitleBorderSize(0);
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(111111);
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(0);
    gStyle->SetFrameFillStyle(0);
    gStyle->SetFrameFillColor(0);
    gStyle->SetAxisColor(1, "X");
    gStyle->SetAxisColor(1, "Y");
    gStyle->SetTitleColor(1, "X");
    gStyle->SetTitleColor(1, "Y");
    gStyle->SetTitleOffset(1, "X");
    gStyle->SetTitleOffset(1.1, "Y");
    gStyle->SetLabelOffset(0.005,"X");
    gStyle->SetLabelOffset(0.005, "Y");
    gStyle->SetTitleX(0.25);
    gStyle->SetTitleW(0.75);
    gStyle->SetTitleH(.092);
    gStyle->SetTitleFont(133,"XYZ");
    gStyle->SetLabelFont(133,"XYZ");
    gStyle->SetLegendFont(133);

    gStyle->SetTitleTextColor(1);
    gStyle->SetTitleStyle(0);
    gStyle->SetLabelSize(.03,"X");
    gStyle->SetLabelSize(.03,"Y");
    gStyle->SetLabelSize(.03,"Z");
    gStyle->SetLineWidth(2);
    gStyle->SetHistLineWidth(2);
    gStyle->SetHistLineColor(1);

    const Int_t NRGBs = 5;
    const Int_t NCont = 64;

    Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
    Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
    Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
    Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
    gStyle->SetNumberContours(NCont);

    gStyle->SetFrameBorderMode(0);
    gStyle->SetCanvasBorderMode(0);
    gStyle->SetPadBorderMode(0);
    gStyle->SetPadColor(0);
    gStyle->SetCanvasColor(0);
    gStyle->SetStatColor(0);
    gStyle->SetPalette(55);

    // Set the paper & margin sizes
    gStyle->SetPaperSize(20,26);
    gStyle->SetPadTopMargin(0.06);
    gStyle->SetPadRightMargin(0.12); // increase for colz plots!!
    gStyle->SetPadBottomMargin(0.16);
    gStyle->SetPadLeftMargin(0.14);

    // Use large fonts
    Int_t lhcbFont = 132;
    gStyle->SetTextFont(lhcbFont);
    gStyle->SetTextSize(0.08);
    gStyle->SetLabelFont(lhcbFont,"x");
    gStyle->SetLabelFont(lhcbFont,"y");
    gStyle->SetLabelFont(lhcbFont,"z");
    gStyle->SetLabelSize(0.042,"x");
    gStyle->SetLabelSize(0.042,"y");
    gStyle->SetLabelSize(0.042,"z");
    gStyle->SetTitleFont(lhcbFont, "x");
    gStyle->SetTitleFont(lhcbFont, "y");
    gStyle->SetTitleFont(lhcbFont, "z");
    gStyle->SetLabelSize(0.042,"x");
    gStyle->SetLabelSize(0.042,"y");
    gStyle->SetLabelSize(0.042,"z");
    gStyle->SetTitleSize(0.06,"x");
    gStyle->SetTitleSize(0.06,"y");
    gStyle->SetTitleSize(0.06,"z");

    // Use bold lines and markers
    Double_t lhcbWidth = 2.00;
    gStyle->SetLineWidth(lhcbWidth);
    gStyle->SetFrameLineWidth(2.0);
    gStyle->SetHistLineWidth(lhcbWidth*1.5);
    gStyle->SetFuncWidth(lhcbWidth*1.5);

    gStyle->SetGridStyle(1);
    gStyle->SetGridColor(kGray+1);
    gStyle->SetLineStyleString(2,"[12 12]");
    gStyle->SetMarkerStyle(20);
    gStyle->SetMarkerSize(0);
    gStyle->SetLegendBorderSize(0);
    gStyle->SetLabelOffset(0.015);

    // Show only nent, mean, rms
    gStyle->SetOptStat(1110);
    gStyle->SetOptTitle(0);

    // Show probability, parameters and errors
    gStyle->SetOptFit(1011);

    // Stats box style
    gStyle->SetStatBorderSize(1);
    gStyle->SetStatFont(lhcbFont);
    gStyle->SetStatFontSize(0.04);
    gStyle->SetStatX(0.9);
    gStyle->SetStatY(0.9);
    gStyle->SetStatW(0.25);
    gStyle->SetStatH(0.15);

    gStyle->SetFrameFillStyle(0);
    gStyle->SetFrameFillColor(0);
    gStyle->SetStatStyle(0);
    gStyle->SetStatBorderSize(0);
    gStyle->SetPadTickX(1);
    gStyle->SetPadTickY(1);

    // Histogram divisions: only 5 in x to avoid label overlaps
    gStyle->SetNdivisions(505,"x");
    gStyle->SetNdivisions(510,"y");

    TPaveText *lhcbName = new TPaveText(0.65,0.8,0.9,0.9,"BRNDC");
    lhcbName->SetFillColor(0);
    lhcbName->SetTextAlign(12);
    lhcbName->SetBorderSize(0);
    lhcbName->AddText("LHCb");

    TText *lhcbLabel = new TText();
    lhcbLabel->SetTextFont(lhcbFont);
    lhcbLabel->SetTextColor(1);
    lhcbLabel->SetTextSize(0.04);
    lhcbLabel->SetTextAlign(12);

    TLatex *lhcbLatex = new TLatex();
    lhcbLatex->SetTextFont(lhcbFont);
    lhcbLatex->SetTextColor(1);
    lhcbLatex->SetTextSize(0.04);
    lhcbLatex->SetTextAlign(12);

}
