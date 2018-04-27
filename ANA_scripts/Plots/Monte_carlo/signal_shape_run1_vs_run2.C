// Script to compare Run 1 and Run 2 fit shapes
void signal_shape_run1_vs_run2() {

    // Open histograms
    TString dir = "~/analysis/B02DKstar/Fit_monte_carlo/Histograms/";
    gROOT->ForceStyle();
    TFile * file_run1 = TFile::Open(dir + "signal_run1.root", "READ");
    TH1F * hist_run1 = (TH1F*)file_run1->Get("fit");
    TFile * file_run2 = TFile::Open(dir + "signal_Kpi.root", "READ");
    TH1F * hist_run2 = (TH1F*)file_run2->Get("fit");

    // Normalise
    hist_run2->Scale(1/hist_run2->Integral());
    hist_run1->Scale(1/hist_run1->Integral());

    // Style
    hist_run2->SetLineColor(kRed);
    hist_run1->SetLineColor(ANABlue);
    hist_run1->SetLineStyle(2);

    // Tighten range
    hist_run2->GetXaxis()->SetRangeUser(5200, 5350);

    // Labels
    hist_run2->SetStats(kFALSE);
    hist_run2->GetXaxis()->SetTitle("#it{m}(DK^{*0}) [MeV/#it{c}^{2}]");
    hist_run2->GetYaxis()->SetTitle("Normalised candidates");
    hist_run2->GetYaxis()->SetTitleOffset(1.35);

    // Legend 
    TLegend leg(0.65, 0.65, 0.85, 0.85);
    leg.AddEntry(hist_run1, "Run 1", "L");
    leg.AddEntry(hist_run2, "Run 2", "L");
    
    // Plot
    TCanvas * canv = new TCanvas("canvas", "", 900, 600);
    hist_run1->Draw("C SAME");
    hist_run2->Draw("C SAME");
    leg.Draw();

    // Save
    canv->SaveAs("~/analysis/B02DKstar/ANA_resources/Plots/Monte_carlo/"
            "run1_vs_run2_signal.pdf");
    file_run1->Close();
    file_run2->Close();

}
