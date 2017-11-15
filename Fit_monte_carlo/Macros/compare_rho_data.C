#include "TFile.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TROOT.h"


void compare_rho_data() {

    // Open files
    gROOT->ForceStyle();
    TFile * file1 = TFile::Open("/home/pullen/analysis/B02DKstar/Fit_monte_carlo/Histograms/rho_noPIDcut_Kpi.root", "READ");
    TFile * file2 = TFile::Open("/home/pullen/analysis/B02DKstar/Fit_monte_carlo/Histograms/rho_KstarK_PIDcut_Kpi.root", "READ");
    TFile * file3 = TFile::Open("/home/pullen/analysis/B02DKstar/Fit_monte_carlo/Histograms/rho_all_PIDcut_Kpi.root", "READ");

    // Get fit histograms
    TH1F * fit1 = (TH1F*)file1->Get("data");
    TH1F * fit2 = (TH1F*)file2->Get("data");
    TH1F * fit3 = (TH1F*)file3->Get("data");
    
    // Normalise
    //fit1->Scale(1/fit1->Integral());
    //fit2->Scale(1/fit2->Integral());
    //fit3->Scale(1/fit3->Integral());

    // Set styles
    fit1->SetLineColor(kRed);
    fit2->SetLineColor(kBlue);
    fit3->SetLineColor(kGreen);
    fit1->SetLineWidth(1);
    fit2->SetLineWidth(1);
    fit3->SetLineWidth(1);

    // Make canvas and draw
    gROOT->ForceStyle();
    TCanvas * canv = new TCanvas("compare", "", 500, 400);
    fit3->Draw("HIST");
    fit1->Draw("HIST SAME");
    fit2->Draw("HIST SAME");

    // Make legend
    TLegend * leg = new TLegend(0.5, 0.75, 0.8, 0.9);
    leg->AddEntry(fit1, "No PID cuts");
    leg->AddEntry(fit2, "KstarK PID > 3 cut");
    leg->AddEntry(fit3, "All PID cuts");
    leg->SetFillStyle(0);
    leg->Draw();

    // Save 
    canv->SaveAs("/home/pullen/analysis/B02DKstar/Fit_monte_carlo/Plots/compare_rho_data_noScale.pdf");
    delete canv;
}
