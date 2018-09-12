// ===============================================
// Script to plot effects of PID cut on rho sample
// ===============================================
void plot_rho_after_PID_cut() {

    // Open files
    TChain * chain = new TChain("DecayTree");
    chain->Add("/data/lhcb/users/pullen/B02DKstar/MC/backgrounds/rho/"
            "2016_down/Kpi_selected_resampled.root");
    chain->Add("/data/lhcb/users/pullen/B02DKstar/MC/backgrounds/rho/"
            "2016_up/Kpi_selected_resampled.root");

    // Make histograms
    TH1F * hist_full = new TH1F("hist_full", "", 50, 5000, 5800);
    TH1F * hist_cut = new TH1F("hist_cut", "", 50, 5000, 5800);
    chain->Draw("Bd_ConsD_MD >> hist_full");
    chain->Draw("Bd_ConsD_MD >> hist_cut", "KstarK_PIDK > 5");

    // Settings
    hist_full->SetLineWidth(1);
    hist_full->SetLineColor(kBlack);
    hist_full->GetXaxis()->SetTitle("#it{m}(DK^{*0}) [MeV/#it{c}^{2}]");
    hist_full->GetYaxis()->SetTitle("Candidates / (16 MeV/#it{c}^{2})");
    hist_cut->SetLineWidth(1);
    hist_cut->SetLineColor(kRed);
    hist_cut->SetFillColor(kRed);
    hist_cut->SetFillStyle(3354);

    // Draw
    TCanvas * canvas = new TCanvas ("canvas", "", 900, 600);
    hist_full->Draw("HIST");
    hist_cut->Draw("HIST SAME");
    gPad->RedrawAxis();
    canvas->SaveAs("cut_rho.pdf");

    std::cout << "Entries before cut: " << hist_full->Integral() << std::endl;
    std::cout << "Entries after cut: " << hist_cut->Integral() << std::endl;

    // Normalise such that centre bin has the same height
    double max_full = hist_full->GetMaximum();
    double max_cut = hist_cut->GetMaximum();
    hist_cut->Scale(max_full/max_cut);

    // Make legend
    TLegend leg(0.55, 0.7, 0.85, 0.85);
    leg.AddEntry(hist_full, "No PID cut", "L");
    leg.AddEntry(hist_cut, "DLL_{K} > 5", "F");

    // Draw
    canvas->Clear();
    hist_cut->Draw("HIST");
    hist_full->Draw("C SAME");
    leg.Draw();
    gPad->RedrawAxis();
    canvas->SaveAs("cut_rho_normalized.pdf");


}
