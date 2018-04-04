// =============================================
// Plot ROC curve for BDT method vs. BDTG method
// =============================================
void BDT_vs_BDTG() {

    // Open file and get histograms
    TFile * file = TFile::Open("/home/pullen/analysis/B02DKstar/BDT/Outputs/"
            "Classification_Kpi_run2_test.root", "READ");
    gROOT->ForceStyle();
    TH1F * roc_bdt = (TH1F*)file->Get("Method_BDT/BDT_Kpi_run2_test/"
            "MVA_BDT_Kpi_run2_test_effBvsS");
    TH1F * roc_bdtg = (TH1F*)file->Get("Method_BDT/BDTG_Kpi_run2_test/"
            "MVA_BDTG_Kpi_run2_test_effBvsS");

    // Draw
    TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
    roc_bdt->SetLineColor(ANABlue);
    roc_bdt->SetLineWidth(1);
    // roc_bdt->GetXaxis()->SetRangeUser(0.6, 1);
    // roc_bdt->GetYaxis()->SetRangeUser(0, 0.1);
    roc_bdt->GetXaxis()->SetTitle("Signal efficiency");
    roc_bdt->GetYaxis()->SetTitle("Background efficiency");
    roc_bdtg->SetLineColor(kRed + 2);
    roc_bdtg->SetLineWidth(1);
    roc_bdt->Draw("L");
    roc_bdtg->Draw("L SAME");
    TLegend * leg = new TLegend(0.25, 0.6, 0.45, 0.85);
    leg->AddEntry(roc_bdt, "BDT", "L");
    leg->AddEntry(roc_bdtg, "BDTG", "L");
    leg->Draw();
    canvas->SaveAs("/home/pullen/analysis/B02DKstar/ANA_plots/Plots/Selection/BDT/"
            "BDT_vs_BDTG.pdf");
}
