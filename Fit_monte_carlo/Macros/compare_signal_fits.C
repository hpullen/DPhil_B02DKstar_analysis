#include "TFile.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TROOT.h"


void compare_signal_fits(std::string mode1, std::string mode2) {

    // Open files
    gROOT->ForceStyle();
    TFile * file1 = TFile::Open(("/home/pullen/analysis/B02DKstar/Fit_monte_carlo/Histograms/signal_" + mode1 + ".root").c_str(), "READ");
    TFile * file2 = TFile::Open(("/home/pullen/analysis/B02DKstar/Fit_monte_carlo/Histograms/signal_" + mode2 + ".root").c_str(), "READ");

    // Get fit histograms
    TH1F * fit1 = (TH1F*)file1->Get("fit");
    TH1F * fit2 = (TH1F*)file2->Get("fit");
    
    // Normalise
    fit1->Scale(fit2->GetMaximum()/fit1->GetMaximum());

    // Set styles
    fit1->SetLineColor(kRed);
    fit2->SetLineColor(kBlue);

    // Make canvas and draw
    gROOT->ForceStyle();
    TCanvas * canv = new TCanvas("compare", "", 500, 400);
    fit1->SetStats(false);
    fit1->Draw("HIST C");
    fit2->Draw("HIST C SAME");

    // Make legend
    TLegend * leg = new TLegend(0.2, 0.75, 0.5, 0.9);
    leg->AddEntry(fit1, "BKGCAT == 0/50/60");
    leg->AddEntry(fit2, "BKGCAT == 0");
    leg->SetFillStyle(0);
    leg->Draw();

    // Save 
    canv->SaveAs(("/home/pullen/analysis/B02DKstar/Fit_monte_carlo/Plots/Comparisons/compare_" + mode1 + "_vs_" + mode2 + ".pdf").c_str());
    delete canv;
}
