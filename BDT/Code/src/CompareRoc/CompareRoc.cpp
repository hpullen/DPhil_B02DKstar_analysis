// System includes
#include <sstream>

// ROOT includes
#include "TAttLine.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TROOT.h"

// Library includes
#include "PlotStyle.hpp"

// Local includes
#include "CompareRoc.hpp"


// ===========
// Constructor
// ===========
CompareRoc::CompareRoc() {

    // Set plot style
    setPlotStyle();
}


// ==========
// Destructor
// ==========
CompareRoc::~CompareRoc() {}


// ===============
// Add a ROC curve
// ===============
void CompareRoc::addCurve(std::string mva_name, std::string mva_type) {

    // Open the file
    TFile * file = TFile::Open(("../Outputs/Classification_" + mva_name + 
                ".root").c_str());

    // Construct histogram name
    std::string method_type;
    if (mva_type == "BDT" || mva_type == "BDTG") {
        method_type = "BDT";
    } else {
        method_type = mva_type;
    }
    std::string fullname = mva_type + "_" + mva_name;
    std::stringstream ss;
    ss << "Method_" << method_type << "/" << fullname << "/MVA_" << fullname 
        << "_effBvsS";

    // Get histogram
    gROOT->ForceStyle();
    TH1D * curve = (TH1D *)file->Get(ss.str().c_str());
    gROOT->ForceStyle();

    // Add histogram to map
    m_curves.emplace(fullname, curve);
}


// =============================
// Make a plot of the ROC curves
// =============================
void CompareRoc::plotCurves(std::string plotname) {

    // Make canvas
    gROOT->ForceStyle(); 
    TCanvas * canv = new TCanvas("canvas", "", 500, 400);

    // Make legend
    TLegend * leg = new TLegend(0.2, 0.6, 0.7, 0.9);
    leg->SetFillColor(0);
    leg->SetFillStyle(0);
    // char * labels[4] = {
        // "Old variables, D^{0} mass sidebands",
        // "New variables, D^{0} mass window",
        // "New variables, D^{0} mass sidebands"
    // // };
    // char * labels[4] = {
        // "Previous analysis",
        // "Current analysis"
    // };

    // Add each curve
    int col = 2;
    int i = 0;
    for (auto curve : m_curves) {
        TH1D * hist = curve.second;
        hist->SetLineColor(col);
        hist->SetLineWidth(1);
        hist->SetStats(kFALSE);
        hist->GetXaxis()->SetRangeUser(0, 1);
        hist->GetXaxis()->SetTitle("Signal efficiency");
        hist->GetYaxis()->SetTitle("Background efficiency");
        hist->GetYaxis()->SetTitleOffset(1);
        gROOT->ForceStyle(); 
        hist->Draw("C SAME");
        leg->AddEntry(hist, curve.first.c_str());
        // leg->AddEntry(hist, labels[i]);
        col++;
        i++;
    }

    // Draw the legend
    leg->Draw();

    // Save the canvas 
    canv->SaveAs(("../Plots/" + plotname + ".pdf").c_str());
    delete canv;
}
