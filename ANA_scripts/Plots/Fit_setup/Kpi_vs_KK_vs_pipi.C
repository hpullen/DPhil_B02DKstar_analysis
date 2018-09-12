// ===============================================================
// Script to make a plot comparing signal fits of KK, Kpi and pipi
// ===============================================================
void Kpi_vs_KK_vs_pipi() {

    // Get histograms
    TString dir = "~/analysis/B02DKstar/Fit_monte_carlo/Histograms/";
    gROOT->ForceStyle();
    TFile * file_Kpi = TFile::Open(dir + "signal_Kpi.root", "READ");
    TH1F * hist_Kpi = (TH1F*)file_Kpi->Get("fit");
    TFile * file_KK = TFile::Open(dir + "signal_KK.root", "READ");
    TH1F * hist_KK = (TH1F*)file_KK->Get("fit");
    TFile * file_pipi = TFile::Open(dir + "signal_pipi.root", "READ");
    TH1F * hist_pipi = (TH1F*)file_pipi->Get("fit");

    // Normalise
    hist_Kpi->Scale(1/hist_Kpi->Integral());
    hist_KK->Scale(1/hist_KK->Integral());
    hist_pipi->Scale(1/hist_pipi->Integral());

    // Style
    hist_Kpi->SetLineColor(kRed);
    hist_KK->SetLineColor(ANAGreen);
    hist_KK->SetLineStyle(2);
    hist_pipi->SetLineColor(ANABlue);
    hist_pipi->SetLineStyle(3);

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
    leg.AddEntry(hist_KK, "D #rightarrow KK", "L");
    leg.AddEntry(hist_pipi, "D #rightarrow #pi#pi", "L");
    
    // Plot
    TCanvas * canv = new TCanvas("canvas", "", 900, 600);
    hist_Kpi->Draw("C");
    hist_KK->Draw("C SAME");
    hist_pipi->Draw("C SAME");
    leg.Draw();

    // Save
    canv->SaveAs("/home/pullen/analysis/B02DKstar/ANA_resources/Plots/Fit_setup/Kpi_vs_KK_vs_pipi.pdf");
    file_Kpi->Close();
    file_KK->Close();
    file_pipi->Close();
}
