// ===============================================================
// Script to make a plot comparing signal fits of KK, Kpi and pipi
// ===============================================================
void Kpi_vs_Kpipipi(TString extra = "") {

    // Get histograms
    TString dir = "~/analysis/B02DKstar/Fit_monte_carlo/Histograms/";
    gROOT->ForceStyle();
    TFile * file_Kpi = TFile::Open(dir + "shared_cruijff" + extra + "_Kpi.root", 
            "READ");
    TH1F * hist_Kpi = (TH1F*)file_Kpi->Get("fit");
    TFile * file_Kpipipi = TFile::Open(dir + "shared_cruijff" + extra + 
            "_Kpipipi.root", "READ");
    TH1F * hist_Kpipipi = (TH1F*)file_Kpipipi->Get("fit");

    // Normalise
    double max_Kpi = hist_Kpi->GetMaximum();
    double max_Kpipipi = hist_Kpipipi->GetMaximum();
    hist_Kpipipi->Scale(max_Kpi/max_Kpipipi);

    // Style
    hist_Kpi->SetLineColor(kRed);
    hist_Kpipipi->SetLineColor(ANABlue);
    hist_Kpipipi->SetLineStyle(2);

    // Tighten range
    hist_Kpi->GetXaxis()->SetRangeUser(5200, 5350);

    // Labels
    hist_Kpi->SetStats(kFALSE);
    hist_Kpi->GetXaxis()->SetTitle("#it{m}(DK^{*0}) [MeV/#it{c}^{2}]");
    hist_Kpi->GetYaxis()->SetTitle("Normalised candidates");
    hist_Kpi->GetYaxis()->SetTitleOffset(1.35);

    // Legend 
    TLegend leg(0.65, 0.65, 0.85, 0.85);
    leg.AddEntry(hist_Kpi, "D #rightarrow K#pi", "L");
    leg.AddEntry(hist_Kpipipi, "D #rightarrow K#pi#pi#pi", "L");
    
    // Plot
    TCanvas * canv = new TCanvas("canvas", "", 900, 600);
    hist_Kpi->Draw("C");
    hist_Kpipipi->Draw("C SAME");
    leg.Draw();

    // Save
    canv->SaveAs("/home/pullen/analysis/B02DKstar/ANA_resources/Plots/Fit_setup/Kpi_vs_Kpipipi" + extra + ".pdf");
    file_Kpi->Close();
    file_Kpipipi->Close();
}

